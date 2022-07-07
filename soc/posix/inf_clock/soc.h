/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _POSIX_SOC_INF_CLOCK_SOC_H
#define _POSIX_SOC_INF_CLOCK_SOC_H

#include <zephyr/toolchain.h>
#include "board_soc.h"
#include "posix_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

void posix_soc_clean_up(void);

/**
 * NATIVE_TASK
 *
 * For native_posix, register a function to be called at particular moments
 * during the native_posix execution.
 *
 * There is 5 choices for when the function will be called (level):
 * * PRE_BOOT_1: Will be called before the command line parameters are parsed,
 *   or the HW models are initialized
 *
 * * PRE_BOOT_2: Will be called after the command line parameters are parsed,
 *   but before the HW models are initialized
 *
 * * PRE_BOOT_3: Will be called after the HW models initialization, right before
 *   the "CPU is booted" and Zephyr is started.
 *
 * * FIRST_SLEEP: Will be called the 1st time the CPU is sent to sleep
 *
 * * ON_EXIT: Will be called during termination of the native application
 * execution.
 *
 * The function must take no parameters and return nothing.
 * Note that for the PRE and ON_EXIT levels neither the Zephyr kernel or
 * any Zephyr thread are running.
 */
#define NATIVE_TASK(fn, level, prio)	\
	static void (*_CONCAT(__native_task_, fn)) __used	\
	__attribute__((__section__(".native_" #level STRINGIFY(prio) "_task")))\
	= fn


#define _NATIVE_PRE_BOOT_1_LEVEL	0
#define _NATIVE_PRE_BOOT_2_LEVEL	1
#define _NATIVE_PRE_BOOT_3_LEVEL	2
#define _NATIVE_FIRST_SLEEP_LEVEL	3
#define _NATIVE_ON_EXIT_LEVEL		4

/**
 * @brief Run the set of special native tasks corresponding to the given level
 *
 * @param level One of _NATIVE_*_LEVEL as defined in soc.h
 */
void run_native_tasks(int level);

/**
 * @brief Convert a devicetree GPIO phandle+specifier to PSEL value
 *
 * Various peripherals in nRF SoCs have pin select registers, which
 * usually have PSEL in their names. The low bits of these registers
 * generally look like this in the register map description:
 *
 *     Bit number     5 4 3 2 1 0
 *     ID             B A A A A A
 *
 *     ID  Field  Value    Description
 *     A   PIN    [0..31]  Pin number
 *     B   PORT   [0..1]   Port number
 *
 * Examples:
 *
 * - pin P0.4 has "PSEL value" 4 (B=0 and A=4)
 * - pin P1.5 has "PSEL value" 37 (B=1 and A=5)
 *
 * This macro converts a devicetree GPIO phandle array value
 * "<&gpioX pin ...>" to a "PSEL value".
 *
 * Note: in Nordic SoC devicetrees, "gpio0" means P0, and "gpio1"
 * means P1. This is encoded in the "port" property of each GPIO node.
 *
 * Examples:
 *
 *     foo: my-node {
 *             tx-gpios = <&gpio0 4 ...>;
 *             rx-gpios = <&gpio0 5 ...>, <&gpio1 5 ...>;
 *     };
 *
 *     NRF_DT_GPIOS_TO_PSEL_BY_IDX(DT_NODELABEL(foo), tx_gpios, 0) // 0 + 4 = 4
 *     NRF_DT_GPIOS_TO_PSEL_BY_IDX(DT_NODELABEL(foo), rx_gpios, 1) // 32 + 5 = 37
 */
#define NRF_DT_GPIOS_TO_PSEL_BY_IDX(node_id, prop, idx)                                            \
	((DT_PROP_BY_PHANDLE_IDX(node_id, prop, idx, port) << 5) |                                 \
	 (DT_GPIO_PIN_BY_IDX(node_id, prop, idx) & 0x1F))

/**
 * @brief Equivalent to NRF_DT_GPIOS_TO_PSEL_BY_IDX(node_id, prop, 0)
 */
#define NRF_DT_GPIOS_TO_PSEL(node_id, prop) NRF_DT_GPIOS_TO_PSEL_BY_IDX(node_id, prop, 0)

/**
 * @brief Equivalent to NRF_DT_GPIOS_TO_PSEL_BY_IDX(node_id, prop, 0)
 */
#define NRF_DT_GPIOS_TO_PSEL(node_id, prop) NRF_DT_GPIOS_TO_PSEL_BY_IDX(node_id, prop, 0)

/**
 * If the node has the property, expands to
 * NRF_DT_GPIOS_TO_PSEL(node_id, prop). The default_value argument is
 * not expanded in this case.
 *
 * Otherwise, expands to default_value.
 */
#define NRF_DT_GPIOS_TO_PSEL_OR(node_id, prop, default_value)                                      \
	COND_CODE_1(DT_NODE_HAS_PROP(node_id, prop), (NRF_DT_GPIOS_TO_PSEL(node_id, prop)),        \
		    (default_value))

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SOC_INF_CLOCK_SOC_H */
