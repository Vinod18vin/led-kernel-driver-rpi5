savedcmd_/home/aiunika-rasp/led_driver/led_driver.mod := printf '%s\n'   led_driver.o | awk '!x[$$0]++ { print("/home/aiunika-rasp/led_driver/"$$0) }' > /home/aiunika-rasp/led_driver/led_driver.mod
