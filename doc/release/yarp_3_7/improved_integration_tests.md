improved_integration_tests {#yarp_3_7}
-----------

* Tests: Added integration tests to check the correct execution of the following commands:
`yarp --version`
`yarpdev --list`
`yarp plugin --all`

* Tests: If no plugins are available, the command 'yarp plugin --all' now returns 0 (OK) instead of 1 (ERROR).
