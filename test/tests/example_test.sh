#!/usr/bin/env bash

function test_capture_stdin() {
    assert_same "time: $(date '+%H:%M')" "$(echo -n 'time: ' | ./terminal_interceptor cat)"
}
