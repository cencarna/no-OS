/***************************************************************************//**
 *   @file   pulsar_adc.h
 *   @brief  Header file for pulsar_adc Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef SRC_PULSAR_ADC_H_
#define SRC_PULSAR_ADC_H_

#include <stdbool.h>

#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#include "clk_axi_clkgen.h"
#include "no_os_pwm.h"
#else
#include "no_os_spi.h"
#endif
#include "no_os_gpio.h"

#define PULSAR_ADC_READ_COMMAND	0x54
#define PULSAR_ADC_WRITE_COMMAND	0x14
#define PULSAR_ADC_RESERVED_MSK	0xE0

#define PULSAR_ADC_TURBO_MODE(x)		(((x) & 0x1) << 1)
#define PULSAR_ADC_HIGH_Z_MODE(x)		(((x) & 0x1) << 2)
#define PULSAR_ADC_SPAN_COMPRESSION(x)	(((x) & 0x1) << 3)
#define PULSAR_ADC_EN_STATUS_BITS(x)	(((x) & 0x1) << 4)

enum pulsar_adc_supported_dev_ids {
	ID_AD4000,
	ID_AD4001,
	ID_AD4002,
	ID_AD4003,
	ID_AD4004,
	ID_AD4005,
	ID_AD4006,
	ID_AD4007,
	ID_AD4008,
	ID_AD4010,
	ID_AD4011,
	ID_AD4020,
	ID_AD4021,
	ID_AD4022,
	ID_ADAQ4003,
	ID_AD7690,
	ID_AD7691,
	ID_AD7693,
	ID_AD7942,
	ID_AD7944,
	ID_AD7946,
	ID_AD7980,
	ID_AD7982,
	ID_AD7983,
	ID_AD7984,
	ID_AD7985,
	ID_AD7986,
};

struct pulsar_adc_dev_info {
	uint16_t resolution;
	char sign;
	uint16_t max_rate;
};

struct pulsar_adc_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* Clock gen for hdl design structure */
	struct axi_clkgen *clkgen;
	/* Trigger conversion PWM generator descriptor */
	struct no_os_pwm_desc *trigger_pwm_desc;
	/** Conversion Start GPIO descriptor. */
	struct no_os_gpio_desc *gpio_cnv;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum pulsar_adc_supported_dev_ids dev_id;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** device info */
	const struct pulsar_adc_dev_info *dev_info;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/* enable offload */
	bool offload_enable;
};

struct pulsar_adc_init_param {
	/* SPI */
	struct no_os_spi_init_param *spi_init;
	/* PWM generator init structure */
	struct no_os_pwm_init_param *trigger_pwm_init;
	/* Clock gen for hdl design init structure */
	struct axi_clkgen_init *clkgen_init;
	/* Clock generator rate */
	uint32_t axi_clkgen_rate;
	/** Conversion Start GPIO configuration. */
	struct no_os_gpio_init_param *gpio_cnv;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum pulsar_adc_supported_dev_ids dev_id;
	/** SPI module offload init */
	struct spi_engine_offload_init_param *offload_init_param;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/* buffer to store data samples */
	uint32_t *buffer;
	/* buffer size */
	uint32_t buffer_size;
	bool offload_enable;
	bool turbo_mode;
	bool high_z_mode;
	bool span_compression;
	bool en_status_bits;
};

int32_t pulsar_adc_spi_reg_read(struct pulsar_adc_dev *dev,
				uint8_t *reg_data);
int32_t pulsar_adc_spi_reg_write(struct pulsar_adc_dev *dev,
				 uint8_t reg_data);
int32_t pulsar_adc_init(struct pulsar_adc_dev **device,
			struct pulsar_adc_init_param *init_param);
int32_t pulsar_adc_remove(struct pulsar_adc_dev *dev);
/* read data samples */
int32_t pulsar_adc_read_data(struct pulsar_adc_dev *dev, uint32_t *buf,
			     uint16_t samples);
#endif /* SRC_PULSAR_ADC_H_ */
