/*
 * Copyright (c) 2015, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <mpi.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gtest/gtest.h"
#include "geopm.h"
#include "geopm_policy.h"

class MPIProfileTest: public :: testing :: Test
{
    public:
        MPIProfileTest();
        virtual ~MPIProfileTest();
    protected:
};

MPIProfileTest::MPIProfileTest()
{
    geopm_policy_c *policy = NULL;
    shm_unlink("/geopm_runtime_test");
    for (int i = 0; i < 16; i++) {
        std::string cleanup("/geopm_runtime_test_" + std::to_string(i));
        shm_unlink(cleanup.c_str());
    }
    EXPECT_EQ(0, geopm_policy_create("", "profile_runtime_policy", &policy));
    EXPECT_EQ(0, geopm_policy_mode(policy, GEOPM_MODE_PERF_BALANCE_DYNAMIC));
    EXPECT_EQ(0, geopm_policy_power(policy, 2000));
    EXPECT_EQ(0, geopm_policy_write(policy));
    EXPECT_EQ(0, geopm_policy_destroy(policy));
}

MPIProfileTest::~MPIProfileTest()
{
    remove("profile_runtime_policy");
}

TEST_F(MPIProfileTest, runtime)
{
    struct geopm_policy_c *policy = NULL;
    struct geopm_prof_c *prof;
    struct geopm_ctl_c *ctl;
    pthread_t ctl_thread;
    uint64_t region_id[3];
    struct geopm_time_s start, curr;
    double timeout = 0;
    int rank;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (!rank) {
    	ASSERT_EQ(0, geopm_policy_create("profile_runtime_policy", "", &policy));
    }
    ASSERT_EQ(0, geopm_ctl_create(policy, "/geopm_runtime_test", MPI_COMM_WORLD, &ctl));
    ASSERT_EQ(0, geopm_ctl_pthread(ctl, NULL, &ctl_thread));

    MPI_Barrier(MPI_COMM_WORLD);

    ASSERT_EQ(0, geopm_prof_create("runtime_test", 4096, "/geopm_runtime_test", MPI_COMM_WORLD, &prof));

    ASSERT_EQ(0, geopm_prof_region(prof, "loop_one", GEOPM_POLICY_HINT_UNKNOWN, &region_id[0]));
    ASSERT_EQ(0, geopm_prof_enter(prof, region_id[0]));
    ASSERT_EQ(0, geopm_time(&start));
    while (timeout < 1.0) {
        ASSERT_EQ(0, geopm_time(&curr));
        timeout = geopm_time_diff(&start, &curr);
    }
    ASSERT_EQ(0, geopm_prof_exit(prof, region_id[0]));
     
    ASSERT_EQ(0, geopm_prof_region(prof, "loop_two", GEOPM_POLICY_HINT_UNKNOWN, &region_id[1]));
    ASSERT_EQ(0, geopm_prof_enter(prof, region_id[1]));
    ASSERT_EQ(0, geopm_time(&start));
    while (timeout < 2.0) {
        ASSERT_EQ(0, geopm_time(&curr));
        timeout = geopm_time_diff(&start, &curr);
    }
    ASSERT_EQ(0, geopm_prof_exit(prof, region_id[1]));
    
    ASSERT_EQ(0, geopm_prof_region(prof, "loop_three", GEOPM_POLICY_HINT_UNKNOWN, &region_id[2]));
    ASSERT_EQ(0, geopm_prof_enter(prof, region_id[2]));
    ASSERT_EQ(0, geopm_time(&start));
    while (timeout < 3.0) {
        ASSERT_EQ(0, geopm_time(&curr));
        timeout = geopm_time_diff(&start, &curr);
    }
    ASSERT_EQ(0, geopm_prof_exit(prof, region_id[2]));

    ASSERT_EQ(0, geopm_prof_print(prof, "profile_runtime_test.log", 0));
}