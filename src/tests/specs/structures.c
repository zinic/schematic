when(Testing_queues, {
    should(init_empty, {
        Queue *queue = Queue_new();

        assert_equal(queue->head, EMPTY_NODE, "New queue must have a null head.");
        assert_equal(queue->tail, EMPTY_NODE, "New queue must have a null tail.");
    })

    should(append_values, {
        char *value = "Test";
        Queue *queue = Queue_new();
        
        Queue_append(queue, &value);

        assert_equal(
            Queue_poll(queue),
            &value,
            "Queues must hold addresses to new values.");
    })

    should(append_multiple_values, {
        char *value = "Test";
        Queue *queue = Queue_new();
        
        for (int i = 0; i < 4; i++) {
            Queue_append(queue, &value);
        }

        for (int i = 0; i < 4; i++) {
            assert_equal(
                Queue_poll(queue),
                &value,
                "Queues must hold addresses to new values.");
        }
    })
})

when(Testing_stacks, {
    should(init_empty, {
        Stack *stack = Stack_new();

        assert_equal(stack->head, EMPTY_NODE, "New stack must have a null head.");
        assert_equal(stack->tail, EMPTY_NODE, "New stack must have a null tail.");
    })

    should(push_values, {
        char *value = "Test";
        Stack *stack = Stack_new();
        
        Stack_push(stack, &value);

        assert_equal(
            Stack_pop(stack),
            &value,
            "Stacks must hold addresses to new values.");
    })

    should(push_multiple_values, {
        char *value = "Test";
        Stack *stack = Stack_new();
        
        for (int i = 0; i < 4; i++) {
            Stack_push(stack, &value);
        }

        for (int i = 0; i < 4; i++) {
            assert_equal(
                Stack_pop(stack),
                &value,
                "Stacks must hold addresses to new values.");
        }
    })
})