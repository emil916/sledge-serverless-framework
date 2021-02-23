#pragma once

#include "sandbox.h"

bool local_fallback_queue_is_empty();
struct sandbox * local_fallback_queue_get_head();
void local_fallback_queue_remove(struct sandbox *sandbox_to_remove);
struct sandbox *local_fallback_queue_remove_and_return();
struct sandbox *local_fallback_queue_get_next();
void local_fallback_queue_append(struct sandbox *sandbox_to_append);
void local_fallback_queue_preempt(ucontext_t *user_context);
void local_fallback_queue_initialize();