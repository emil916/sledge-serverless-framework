#include "client_socket.h"
#include "local_fallback_queue.h"
//#include "local_runqueue.h"
#include "global_request_scheduler.h"

__thread static struct ps_list_head local_fallback_queue;

bool
local_fallback_queue_is_empty()
{
	return ps_list_head_empty(&local_fallback_queue);
}

/* Get the sandbox at the head of the thread local fallback queue */
struct sandbox *
local_fallback_queue_get_head()
{
	return ps_list_head_first_d(&local_fallback_queue, struct sandbox);
}

/**
 * Removes the thread from the thread-local fallback queue
 * @param sandbox sandbox
 */
void
local_fallback_queue_remove(struct sandbox *sandbox_to_remove)
{
	ps_list_rem_d(sandbox_to_remove);
}

struct sandbox *
local_fallback_queue_remove_and_return()
{
	struct sandbox *sandbox_to_remove = ps_list_head_first_d(&local_fallback_queue, struct sandbox);
	ps_list_rem_d(sandbox_to_remove);
	return sandbox_to_remove;
}

/**
 * Execute the sandbox at the head of the thread local fallback queue
 * If the fallback queue is empty, pull a fresh batch of sandbox requests, instantiate them, and then execute the new head
 * @return the sandbox to execute or NULL if none are available
 */
struct sandbox *
local_fallback_queue_get_next()
{
	struct sandbox_request *sandbox_request;

	// If our local runqueue is empty, try to pull and allocate a sandbox request from the global request scheduler
	// if (local_runqueue_is_empty()) {
	// 	if (global_request_scheduler_remove(&sandbox_request) < 0) goto err;

	// 	struct sandbox *sandbox = sandbox_allocate(sandbox_request);
	// 	if (!sandbox) goto err_allocate;

	// 	sandbox->state = SANDBOX_RUNNABLE;
	// 	local_runqueue_add(sandbox);

	// done:
	// 	return sandbox;
	// err_allocate:
	// 	client_socket_send(sandbox_request->socket_descriptor, 503);
	// 	client_socket_close(sandbox_request->socket_descriptor);
	// 	free(sandbox_request);
	// err:
	// 	sandbox = NULL;
	// 	goto done;
	// }

	/* Execute Round Robin Scheduling Logic */
	// struct sandbox *next_sandbox = local_fallback_queue_remove_and_return();
	// assert(next_sandbox->state != SANDBOX_RETURNED);
	// local_runqueue_add(next_sandbox);

	struct sandbox *next_sandbox = local_fallback_queue_get_head(); // maybe later change to local_fallback_queue_remove_and_return()
	assert(next_sandbox->state != SANDBOX_RETURNED);

	return next_sandbox;
}


/**
 * Append a sandbox to the fallback queue
 * @returns the appended sandbox
 */
void
local_fallback_queue_append(struct sandbox *sandbox_to_append)
{
	assert(ps_list_singleton_d(sandbox_to_append));
	ps_list_head_append_d(&local_fallback_queue, sandbox_to_append);
	if(!local_fallback_queue_is_empty())
		printf("Sandbox successfully added to FALLEN.\n");
}

/**
 * Conditionally checks to see if current sandbox should be preempted.
 * FIFO doesn't preempt, so just return.
 */
void
local_fallback_queue_preempt(ucontext_t *user_context)
{
	return;
}


void
local_fallback_queue_initialize()
{
	ps_list_head_init(&local_fallback_queue);

	/* Register Function Pointers for Abstract Scheduling API */
	// struct local_runqueue_config config = { .add_fn      = local_fallback_queue_append,
	// 	                                .is_empty_fn = local_fallback_queue_is_empty,
	// 	                                .delete_fn   = local_fallback_queue_remove,
	// 	                                .get_next_fn = local_fallback_queue_get_next,
	// 	                                .preempt_fn  = local_fallback_queue_preempt };
//	local_runqueue_initialize(&config);
};
