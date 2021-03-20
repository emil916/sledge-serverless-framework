#pragma once

#include "debuglog.h"
#include "perf_window.h"

struct admissions_info {
	struct perf_window perf_window;
	int                percentile;        /* 50 - 99 */
	int                control_index;     /* Precomputed Lookup index when perf_window is full */
	uint64_t           expected_execution;/* cycles */
	uint64_t           estimate;          /* cycles */
	uint64_t           relative_deadline; /* Relative deadline in cycles. This is duplicated state */
};

/**
 * Initializes perf window
 * @param self
 */
static inline void
admissions_info_initialize(struct admissions_info *self, int percentile, uint64_t expected_execution,
                           uint64_t relative_deadline)
{
#ifdef ADMISSIONS_CONTROL

	self->relative_deadline = relative_deadline;
	self->expected_execution = expected_execution;
	self->estimate          = admissions_control_calculate_estimate(expected_execution, relative_deadline);
	debuglog("Initial Estimate: %lu\n", self->estimate);
	assert(self != NULL);

	perf_window_initialize(&self->perf_window);

	if (unlikely(percentile < 50 || percentile > 99)) panic("Invalid admissions percentile");
	self->percentile = percentile;

	self->control_index = PERF_WINDOW_BUFFER_SIZE * percentile / 100;
#ifdef LOG_ADMISSIONS_CONTROL
	debuglog("Percentile: %d\n", self->percentile);
	debuglog("Control Index: %d\n", self->control_index);
#endif
#endif
}

/*
 * Adds an execution value to the perf window and calculates and caches and updated estimate
 * @param self
 * @param execution_duration
 */
static inline void
admissions_info_update(struct admissions_info *self, uint64_t execution_duration)
{
#ifdef ADMISSIONS_CONTROL
	assert(!software_interrupt_is_enabled());
	struct perf_window *perf_window = &self->perf_window;

	LOCK_LOCK(&self->perf_window.lock);
	perf_window_add(perf_window, execution_duration);
	uint64_t estimated_execution = perf_window_get_percentile(perf_window, self->percentile, self->control_index);
	self->expected_execution = estimated_execution;
	self->estimate = admissions_control_calculate_estimate(estimated_execution, self->relative_deadline);
	LOCK_UNLOCK(&self->perf_window.lock);
#endif
}
