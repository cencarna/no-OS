/***************************************************************************//**
*   @file   iio_lt3074.c
*   @brief  Source file for the LT3074 IIO Driver
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "lt3074.h"
#include "iio_lt3074.h"

static const int32_t lt3074_margin_avail[] = {
	0,
	1,
	3,
	5,
	10,
	15,
	20,
	25,
	30,
};

enum lt3074_iio_chan_type {
	LT3074_IIO_VIN_CHAN,
	LT3074_IIO_VOUT_CHAN,
	LT3074_IIO_IOUT_CHAN,
	LT3074_IIO_TEMP_CHAN,
	LT3074_IIO_VBIAS_CHAN,
};

static struct iio_device lt3074_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int32_t lt3074_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	uint16_t word;
	uint8_t byte;
	uint8_t block[6] = {0};

	switch (reg) {
	case LT3074_PAGE:
	case LT3074_OPERATION:
	case LT3074_ON_OFF_CONFIG:
	case LT3074_WRITE_PROTECT:
	case LT3074_STATUS_BYTE:
	case LT3074_STATUS_VOUT:
	case LT3074_STATUS_IOUT:
	case LT3074_STATUS_INPUT:
	case LT3074_STATUS_TEMPERATURE:
	case LT3074_STATUS_CML:
	case LT3074_STATUS_MFR_SPECIFIC:
	case LT3074_MFR_MARGIN:
	case LT3074_MFR_RAIL_ADDRESS:
	case LT3074_CAPABILITY:
	case LT3074_VOUT_MODE:
	case LT3074_IOUT_OC_FAULT_RESPONSE:
	case LT3074_REVISION:
	case LT3074_MFR_DEFAULT_CONFIG:
		ret = lt3074_read_byte(lt3074, (uint8_t)reg, &byte);
		if (ret)
			return ret;

		*readval = (uint32_t)byte;

		return 0;
	case LT3074_VOUT_OV_WARN_LIMIT:
	case LT3074_VOUT_UV_WARN_LIMIT:
	case LT3074_IOUT_OC_FAULT_LIMIT:
	case LT3074_OT_WARN_LIMIT:
	case LT3074_VIN_OV_WARN_LIMIT:
	case LT3074_VIN_UV_WARN_LIMIT:
	case LT3074_STATUS_WORD:
	case LT3074_MFR_READ_VBIAS:
	case LT3074_MFR_BIAS_OV_WARN_LIMIT:
	case LT3074_MFR_BIAS_UV_WARN_LIMIT:
	case LT3074_MFR_IOUT_MIN_WARN_LIMIT:
	case LT3074_READ_VIN:
	case LT3074_READ_VOUT:
	case LT3074_READ_IOUT:
	case LT3074_READ_TEMPERATURE_1:
	case LT3074_IC_DEVICE_REV:
	case LT3074_MFR_SPECIAL_ID:
		ret = lt3074_read_word(lt3074, (uint8_t)reg, &word);
		if (ret)
			return ret;

		*readval = (uint32_t)word;

		return 0;
	case LT3074_IC_DEVICE_ID:
		ret = lt3074_read_block_data(lt3074, (uint8_t)reg,
					     &block[0], 6);
		if (ret)
			return ret;

		*readval = no_os_get_unaligned_be32(block);

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Result of the writing procedure.
*/
static int32_t lt3074_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;

	switch (reg) {
	case LT3074_PAGE:
	case LT3074_OPERATION:
	case LT3074_ON_OFF_CONFIG:
	case LT3074_WRITE_PROTECT:
	case LT3074_STATUS_BYTE:
	case LT3074_STATUS_VOUT:
	case LT3074_STATUS_IOUT:
	case LT3074_STATUS_INPUT:
	case LT3074_STATUS_TEMPERATURE:
	case LT3074_STATUS_CML:
	case LT3074_STATUS_MFR_SPECIFIC:
	case LT3074_MFR_MARGIN:
	case LT3074_MFR_RAIL_ADDRESS:
		return lt3074_write_byte(lt3074, (uint8_t)reg,
					 (uint8_t)writeval);
	case LT3074_CLEAR_FAULTS:
	case LT3074_MFR_RESET:
		return lt3074_send_byte(dev, (uint8_t) reg);
	case LT3074_VOUT_OV_WARN_LIMIT:
	case LT3074_VOUT_UV_WARN_LIMIT:
	case LT3074_IOUT_OC_FAULT_LIMIT:
	case LT3074_OT_WARN_LIMIT:
	case LT3074_VIN_OV_WARN_LIMIT:
	case LT3074_VIN_UV_WARN_LIMIT:
	case LT3074_STATUS_WORD:
	case LT3074_MFR_READ_VBIAS:
	case LT3074_MFR_BIAS_OV_WARN_LIMIT:
	case LT3074_MFR_BIAS_UV_WARN_LIMIT:
	case LT3074_MFR_IOUT_MIN_WARN_LIMIT:
		return lt3074_write_word(lt3074, (uint8_t)reg,
					 (uint16_t)writeval);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	int32_t value;
	uint16_t word;

	switch (channel->address) {
	case LT3074_IIO_VIN_CHAN:
		ret = lt3074_read_word(lt3074, LT3074_READ_VIN, &word);
		break;
	case LT3074_IIO_IOUT_CHAN:
		ret = lt3074_read_word(lt3074, LT3074_READ_IOUT, &word);
		break;
	case LT3074_IIO_VOUT_CHAN:
		ret = lt3074_read_word(lt3074, LT3074_READ_VOUT, &word);
		break;
	case LT3074_IIO_TEMP_CHAN:
		ret = lt3074_read_word(lt3074, LT3074_READ_TEMPERATURE_1,
				       &word);
		break;
	case LT3074_IIO_VBIAS_CHAN:
		ret = lt3074_read_word(lt3074, LT3074_MFR_READ_VBIAS, &word);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	value = (int32_t)word;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int val, ret;
	int32_t vals[2];
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;

	switch (channel->address) {
	case LT3074_IIO_VIN_CHAN:
		ret = lt3074_read_value(lt3074, LT3074_VIN, &val);
		break;
	case LT3074_IIO_IOUT_CHAN:
		ret = lt3074_read_value(lt3074, LT3074_IOUT, &val);
		break;
	case LT3074_IIO_VOUT_CHAN:
		ret = lt3074_read_value(lt3074, LT3074_VOUT, &val);
		break;
	case LT3074_IIO_TEMP_CHAN:
		ret = lt3074_read_value(lt3074, LT3074_TEMP, &val);
		break;
	case LT3074_IIO_VBIAS_CHAN:
		ret = lt3074_read_value(lt3074, LT3074_VBIAS, &val);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	vals[0] = (int32_t)val;
	vals[1] = MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the read request for status debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_status(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	int32_t value;
	uint16_t status_word;
	uint8_t status_byte;

	if (priv == LT3074_STATUS_WORD) {
		ret = lt3074_read_word(lt3074, LT3074_STATUS_WORD, &status_word);
		if (ret)
			return ret;

		value = (int32_t)status_word;
	} else {
		ret = lt3074_read_byte(lt3074, (uint8_t)priv, &status_byte);
		if (ret)
			return ret;

		value = (int32_t)status_byte;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

/**
 * @brief Handles the read request for vout margin global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_vout_margin(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	uint8_t data;

	if (!dev)
		return -EINVAL;

	if (!iio_lt3074->lt3074_dev)
		return -EINVAL;

	ret = lt3074_read_byte(lt3074, LT3074_MFR_MARGIN, &data);
	if (ret)
		return ret;

	data = no_os_field_get(priv, data);
	data = no_os_clamp(data, 0, 8);

	return sprintf(buf, "%d ", lt3074_margin_avail[data]);
}

/**
 * @brief Handles the read request for vout_margin_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_margin_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt3074_margin_avail); i++)
		length += sprintf(buf + length, "%d ",
				  lt3074_margin_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for vout margin global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_write_vout_margin(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	int32_t val;
	uint8_t data, i;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt3074_margin_avail); i++)
		if (val == lt3074_margin_avail[i])
			break;

	if (i == NO_OS_ARRAY_SIZE(lt3074_margin_avail))
		return -EINVAL;

	ret = lt3074_read_byte(lt3074, LT3074_MFR_MARGIN, &data);
	if (ret)
		return ret;

	data &= ~priv;
	data |= no_os_field_prep(priv, i);

	return lt3074_write_byte(lt3074, LT3074_MFR_MARGIN, data);
}

/**
 * @brief Handles the read request for limits global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_read_limits(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret, word;
	int32_t vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt3074->lt3074_dev)
		return -EINVAL;

	ret = lt3074_read_word_data(lt3074, priv, &word);
	if (ret)
		return ret;

	vals[0] = (int32_t)(word / MILLI);
	vals[1] = (int32_t)((word * MILLI) % MICRO);

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

/**
 * @brief Handles the write request for limits global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt3074_iio_write_limits(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int limit;
	int32_t val1, val2;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);

	limit = (int)(val1 * MILLI + val2 / MILLI);

	return lt3074_set_limit(lt3074, (enum lt3074_limit_type)priv, limit);
}

/**
 * @brief Initializes the LT3074 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int lt3074_iio_init(struct lt3074_iio_desc **iio_desc,
		    struct lt3074_iio_desc_init_param *init_param)
{
	struct lt3074_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->lt3074_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = lt3074_init(&descriptor->lt3074_dev,
			  init_param->lt3074_init_param);
	if (ret)
		goto dev_err;

	descriptor->iio_dev = &lt3074_iio_dev;

	*iio_desc = descriptor;

	return 0;

dev_err:
	lt3074_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int lt3074_iio_remove(struct lt3074_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	lt3074_remove(iio_desc->lt3074_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute lt3074_chan_attrs[] = {
	{
		.name = "raw",
		.show = lt3074_iio_read_raw,
	},
	{
		.name = "scale",
		.show = lt3074_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt3074_global_attrs[] = {
	{
		.name = "vout_margin_low",
		.show = lt3074_iio_read_vout_margin,
		.store = lt3074_iio_write_vout_margin,
		.priv = LT3074_MARGIN_LOW_MSK
	},
	{
		.name = "vout_margin_high",
		.show = lt3074_iio_read_vout_margin,
		.store = lt3074_iio_write_vout_margin,
		.priv = LT3074_MARGIN_HIGH_MSK
	},
	{
		.name = "vout_margin_available",
		.show = lt3074_iio_read_margin_available
	},
	{
		.name = "vout_ov_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VOUT_OV_WARN_LIMIT_TYPE
	},
	{
		.name = "vout_uv_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VOUT_UV_WARN_LIMIT_TYPE
	},
	{
		.name = "iout_oc_fault_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_IOUT_OC_FAULT_LIMIT_TYPE
	},
	{
		.name = "ot_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_OT_WARN_LIMIT_TYPE
	},
	{
		.name = "vin_ov_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VIN_OV_WARN_LIMIT_TYPE
	},
	{
		.name = "vin_uv_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VIN_UV_WARN_LIMIT_TYPE
	},
	{
		.name = "vbias_ov_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VBIAS_OV_WARN_LIMIT_TYPE
	},
	{
		.name = "vbias_uv_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_VBIAS_UV_WARN_LIMIT_TYPE
	},
	{
		.name = "iout_min_warn_limit",
		.show = lt3074_iio_read_limits,
		.store = lt3074_iio_write_limits,
		.priv = LT3074_IOUT_MIN_WARN_LIMIT_TYPE
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt3074_debug_attrs[] = {
	{
		.name = "status_byte",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_BYTE
	},
	{
		.name = "status_vout",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_VOUT
	},
	{
		.name = "status_iout",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_IOUT
	},
	{
		.name = "status_input",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_INPUT
	},
	{
		.name = "status_temperature",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_TEMPERATURE
	},
	{
		.name = "status_cml",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_CML
	},
	{
		.name = "status_mfr_specific",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_MFR_SPECIFIC
	},
	{
		.name = "status_word",
		.show = lt3074_iio_read_status,
		.priv = LT3074_STATUS_WORD
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel lt3074_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VIN_CHAN,
		.address = LT3074_IIO_VIN_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VOUT_CHAN,
		.address = LT3074_IIO_VOUT_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = true,
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT3074_IIO_IOUT_CHAN,
		.address = LT3074_IIO_IOUT_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = true,
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = LT3074_IIO_TEMP_CHAN,
		.address = LT3074_IIO_TEMP_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false,
	},
	{
		.name = "vbias",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VBIAS_CHAN,
		.address = LT3074_IIO_VBIAS_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false,
	},
};

static struct iio_device lt3074_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt3074_channels),
	.channels = lt3074_channels,
	.attributes = lt3074_global_attrs,
	.debug_attributes = lt3074_debug_attrs,
	.debug_reg_read = lt3074_iio_reg_read,
	.debug_reg_write = lt3074_iio_reg_write,
};
