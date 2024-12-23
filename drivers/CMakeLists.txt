# Custom Zephyr function that imports the pmsa003i/ subdirectory if the Kconfig
# option PMSA003I is defined
add_subdirectory_ifdef(CONFIG_PMSA003I pmsa003i)
