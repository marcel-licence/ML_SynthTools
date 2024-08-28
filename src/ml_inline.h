/*
 * Copyright (c) 2024 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
 * @file ml_inline.h
 * @author Marcel Licence
 * @data 02.04.2023
 *
 * @brief   This file includes all required header for inline build
 */

#include <audio_module.h>
#include <blink.h>
#include <es8388.h>
#include <esp32_audio_kit_module.h>
#if (defined ESP32) || (defined ESP8266) || (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
#include <fs/fs_access.h>
#include <fs/fs_common.h>
#include <fs/fs_esp32.h>
#include <fs/fs_esp8266.h>
#include <fs/fs_rp2040.h>
#endif
#include <i2s_interface.h>
#include <i2s_module.h>
#include <midi_interface.h>
#include <midi_stream_player.h>
#include <midi_via_ble.h>
#include <midi_via_usb.h>
#include <ml_board_setup.h>
#include <ml_scope_oled_inline.h>
#include <ml_status.h>
#include <ml_status_inline.h>
#include <samd21_audio_core.h>
#include <usbMidiHost.h>
#include <wm8960.h>
#include <wm8978.h>
