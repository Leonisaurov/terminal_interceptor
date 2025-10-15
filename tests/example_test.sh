#!/usr/bin/env bash

function test_capture_stdout() {
    cd test
    assert_same "What's up?" "$(./terminal_interceptor echo -n wasap)"
}

function test_capture_stdin() {
    cd test
    assert_same $'TIME: 17:08' "$(echo -n 'Time: ' | ./terminal_interceptor echo)"
}
