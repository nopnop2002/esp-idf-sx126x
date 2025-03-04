#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "ra01s.h"

#define TAG "RA01S"

// SPI Stuff
#if CONFIG_SPI2_HOST
#define HOST_ID SPI2_HOST
#elif CONFIG_SPI3_HOST
#define HOST_ID SPI3_HOST
#endif

static spi_device_handle_t SpiHandle;

// Global Stuff
static uint8_t PacketParams[6];
static bool txActive;
static int txLost = 0;
static bool debugPrint;
static int SX126x_SPI_SELECT;
static int SX126x_RESET;
static int SX126x_BUSY;
static int SX126x_TXEN;
static int SX126x_RXEN;

// Arduino compatible macros
#define delayMicroseconds(us) esp_rom_delay_us(us)
#define delay(ms) esp_rom_delay_us(ms*1000)


void LoRaErrorDefault(int error)
{
	if (debugPrint) {
		ESP_LOGE(TAG, "LoRaErrorDefault=%d", error);
	}
	while (true) {
		vTaskDelay(1);
	}
}

__attribute__ ((weak, alias ("LoRaErrorDefault"))) void LoRaError(int error);


void LoRaInit(void)
{
	ESP_LOGI(TAG, "CONFIG_MISO_GPIO=%d", CONFIG_MISO_GPIO);
	ESP_LOGI(TAG, "CONFIG_MOSI_GPIO=%d", CONFIG_MOSI_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCLK_GPIO=%d", CONFIG_SCLK_GPIO);
	ESP_LOGI(TAG, "CONFIG_NSS_GPIO=%d", CONFIG_NSS_GPIO);
	ESP_LOGI(TAG, "CONFIG_RST_GPIO=%d", CONFIG_RST_GPIO);
	ESP_LOGI(TAG, "CONFIG_BUSY_GPIO=%d", CONFIG_BUSY_GPIO);
	ESP_LOGI(TAG, "CONFIG_TXEN_GPIO=%d", CONFIG_TXEN_GPIO);
	ESP_LOGI(TAG, "CONFIG_RXEN_GPIO=%d", CONFIG_RXEN_GPIO);

	SX126x_SPI_SELECT = CONFIG_NSS_GPIO;
	SX126x_RESET = CONFIG_RST_GPIO;
	SX126x_BUSY	= CONFIG_BUSY_GPIO;
	SX126x_TXEN	= CONFIG_TXEN_GPIO;
	SX126x_RXEN	= CONFIG_RXEN_GPIO;
	
	txActive = false;
	debugPrint = false;

	gpio_reset_pin(SX126x_SPI_SELECT);
	gpio_set_direction(SX126x_SPI_SELECT, GPIO_MODE_OUTPUT);
	gpio_set_level(SX126x_SPI_SELECT, 1);

	gpio_reset_pin(SX126x_RESET);
	gpio_set_direction(SX126x_RESET, GPIO_MODE_OUTPUT);
	
	gpio_reset_pin(SX126x_BUSY);
	gpio_set_direction(SX126x_BUSY, GPIO_MODE_INPUT);

	if (SX126x_TXEN != -1) {
		gpio_reset_pin(SX126x_TXEN);
		gpio_set_direction(SX126x_TXEN, GPIO_MODE_OUTPUT);
	}

	if (SX126x_RXEN != -1) {
		gpio_reset_pin(SX126x_RXEN);
		gpio_set_direction(SX126x_RXEN, GPIO_MODE_OUTPUT);
	}

	spi_bus_config_t spi_bus_config = {
		.sclk_io_num = CONFIG_SCLK_GPIO,
		.mosi_io_num = CONFIG_MOSI_GPIO,
		.miso_io_num = CONFIG_MISO_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	esp_err_t ret;
	ret = spi_bus_initialize( HOST_ID, &spi_bus_config, SPI_DMA_CH_AUTO );
	ESP_LOGI(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg = {
		.clock_speed_hz = 9000000,
		.mode = 0,
		.spics_io_num = CONFIG_NSS_GPIO,
		.queue_size = 7,
		.flags = 0,
		.pre_cb = NULL
	};
	//spi_device_handle_t handle;
	ret = spi_bus_add_device( HOST_ID, &devcfg, &SpiHandle);
	ESP_LOGI(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
}

void spi_write_byte(uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = NULL;
		spi_device_transmit( SpiHandle, &SPITransaction );
	}

	return;
}

void spi_read_byte(uint8_t* Datain, uint8_t* Dataout, size_t DataLength )
{
	spi_transaction_t SPITransaction;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Dataout;
		SPITransaction.rx_buffer = Datain;
		spi_device_transmit( SpiHandle, &SPITransaction );
	}

	return;
}

uint8_t spi_transfer(uint8_t address)
{
	uint8_t datain[1];
	uint8_t dataout[1];
	dataout[0] = address;
	//spi_write_byte(dataout, 1 );
	spi_read_byte(datain, dataout, 1 );
	return datain[0];
}


int16_t LoRaBegin(uint32_t frequencyInHz, int8_t txPowerInDbm, float tcxoVoltage, bool useRegulatorLDO) 
{
	if ( txPowerInDbm > 22 )
		txPowerInDbm = 22;
	if ( txPowerInDbm < -3 )
		txPowerInDbm = -3;
	
	Reset();
	
	uint8_t wk[2];
	ReadRegister(SX126X_REG_LORA_SYNC_WORD_MSB, wk, 2); // 0x0740
	uint16_t syncWord = (wk[0] << 8) + wk[1];
	ESP_LOGI(TAG, "syncWord=0x%x", syncWord);
	if (syncWord != SX126X_SYNC_WORD_PUBLIC && syncWord != SX126X_SYNC_WORD_PRIVATE) {
		ESP_LOGE(TAG, "SX126x error, maybe no SPI connection");
		return ERR_INVALID_MODE;
	}

	ESP_LOGI(TAG, "SX126x installed");
	SetStandby(SX126X_STANDBY_RC);

	SetDio2AsRfSwitchCtrl(true);
	ESP_LOGI(TAG, "tcxoVoltage=%f", tcxoVoltage);
	// set TCXO control, if requested
	if(tcxoVoltage > 0.0) {
		SetDio3AsTcxoCtrl(tcxoVoltage, RADIO_TCXO_SETUP_TIME); // Configure the radio to use a TCXO controlled by DIO3
	}

	Calibrate(	SX126X_CALIBRATE_IMAGE_ON
		| SX126X_CALIBRATE_ADC_BULK_P_ON
		| SX126X_CALIBRATE_ADC_BULK_N_ON
		| SX126X_CALIBRATE_ADC_PULSE_ON
		| SX126X_CALIBRATE_PLL_ON
		| SX126X_CALIBRATE_RC13M_ON
		| SX126X_CALIBRATE_RC64K_ON
	);

	ESP_LOGI(TAG, "useRegulatorLDO=%d", useRegulatorLDO);
	if (useRegulatorLDO) {
		SetRegulatorMode(SX126X_REGULATOR_LDO); // set regulator mode: LDO
	} else {
		SetRegulatorMode(SX126X_REGULATOR_DC_DC); // set regulator mode: DC-DC
	}

	SetBufferBaseAddress(0, 0);
#if 0
	// SX1261_TRANCEIVER
	SetPaConfig(0x06, 0x00, 0x01, 0x01); // PA Optimal Settings +15 dBm
	// SX1262_TRANCEIVER
	SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
	// SX1268_TRANCEIVER
	SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
#endif
	SetPaConfig(0x04, 0x07, 0x00, 0x01); // PA Optimal Settings +22 dBm
	SetOvercurrentProtection(60.0);  // current max 60mA for the whole device
	SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U); //0 fuer Empfaenger
	SetRfFrequency(frequencyInHz);
	return ERR_NONE;
}

void FixInvertedIQ(uint8_t iqConfig)
{
	// fixes IQ configuration for inverted IQ
	// see SX1262/SX1268 datasheet, chapter 15 Known Limitations, section 15.4 for details
	// When exchanging LoRa packets with inverted IQ polarity, some packet losses may be observed for longer packets.
	// Workaround: Bit 2 at address 0x0736 must be set to:
	// “0” when using inverted IQ polarity (see the SetPacketParam(...) command)
	// “1” when using standard IQ polarity

	// read current IQ configuration
	uint8_t iqConfigCurrent = 0;
	ReadRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736

	// set correct IQ configuration
	//if(iqConfig == SX126X_LORA_IQ_STANDARD) {
	if(iqConfig == SX126X_LORA_IQ_INVERTED) {
		iqConfigCurrent &= 0xFB; // using inverted IQ polarity
	} else {
		iqConfigCurrent |= 0x04; // using standard IQ polarity
	}

	// update with the new value
	WriteRegister(SX126X_REG_IQ_POLARITY_SETUP, &iqConfigCurrent, 1); // 0x0736
}


void LoRaConfig(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint16_t preambleLength, uint8_t payloadLen, bool crcOn, bool invertIrq) 
{
	SetStopRxTimerOnPreambleDetect(false);
	SetLoRaSymbNumTimeout(0); 
	SetPacketType(SX126X_PACKET_TYPE_LORA); // SX126x.ModulationParams.PacketType : MODEM_LORA
	uint8_t ldro = 0; // LowDataRateOptimize OFF
	SetModulationParams(spreadingFactor, bandwidth, codingRate, ldro);
	
	PacketParams[0] = (preambleLength >> 8) & 0xFF;
	PacketParams[1] = preambleLength;
	if ( payloadLen )
	{
		PacketParams[2] = 0x01; // Fixed length packet (implicit header)
		PacketParams[3] = payloadLen;
	}
	else
	{
		PacketParams[2] = 0x00; // Variable length packet (explicit header)
		PacketParams[3] = 0xFF;
	}

	if ( crcOn )
		PacketParams[4] = SX126X_LORA_CRC_ON;
	else
		PacketParams[4] = SX126X_LORA_CRC_OFF;

	if ( invertIrq )
		PacketParams[5] = 0x01; // Inverted LoRa I and Q signals setup
	else
		PacketParams[5] = 0x00; // Standard LoRa I and Q signals setup

	// fixes IQ configuration for inverted IQ
	FixInvertedIQ(PacketParams[5]);

	WriteCommand(SX126X_CMD_SET_PACKET_PARAMS, PacketParams, 6); // 0x8C

	// Do not use DIO interruptst
	SetDioIrqParams(SX126X_IRQ_ALL, //all interrupts enabled
		SX126X_IRQ_NONE, //interrupts on DIO1
		SX126X_IRQ_NONE, //interrupts on DIO2
		SX126X_IRQ_NONE //interrupts on DIO3
	);

	// Receive state no receive timeoout
	SetRx(0xFFFFFF);
}


void LoRaDebugPrint(bool enable) 
{
	debugPrint = enable;
}


uint8_t LoRaReceive(uint8_t *pData, int16_t len) 
{
	uint8_t rxLen = 0;
	uint16_t irqRegs = GetIrqStatus();
	//uint8_t status = GetStatus();
	
	if( irqRegs & SX126X_IRQ_RX_DONE )
	{
		//ClearIrqStatus(SX126X_IRQ_RX_DONE);
		ClearIrqStatus(SX126X_IRQ_ALL);
		rxLen = ReadBuffer(pData, len);
	}
	
	return rxLen;
}


bool LoRaSend(uint8_t *pData, int16_t len, uint8_t mode)
{
	uint16_t irqStatus;
	bool rv = false;
	
	if ( txActive == false )
	{
		txActive = true;
		if (PacketParams[2] == 0x00) { // Variable length packet (explicit header)
			PacketParams[3] = len;
		}
		WriteCommand(SX126X_CMD_SET_PACKET_PARAMS, PacketParams, 6); // 0x8C
		
		//ClearIrqStatus(SX126X_IRQ_TX_DONE | SX126X_IRQ_TIMEOUT);
		ClearIrqStatus(SX126X_IRQ_ALL);
		
		WriteBuffer(pData, len);
		SetTx(500);

		if ( mode & SX126x_TXMODE_SYNC )
		{
			irqStatus = GetIrqStatus();
			while ( (!(irqStatus & SX126X_IRQ_TX_DONE)) && (!(irqStatus & SX126X_IRQ_TIMEOUT)) )
			{
				delay(1);
				irqStatus = GetIrqStatus();
			}
			if (debugPrint) {
				ESP_LOGI(TAG, "irqStatus=0x%x", irqStatus);
				if (irqStatus & SX126X_IRQ_TX_DONE) {
					ESP_LOGI(TAG, "SX126X_IRQ_TX_DONE");
				}
				if (irqStatus & SX126X_IRQ_TIMEOUT) {
					ESP_LOGI(TAG, "SX126X_IRQ_TIMEOUT");
				}
			}
			txActive = false;
	
			SetRx(0xFFFFFF);
	
			if ( irqStatus & SX126X_IRQ_TX_DONE) {
				rv = true;
			}
		}
		else
		{
			rv = true;
		}
	}
	if (debugPrint) {
		ESP_LOGI(TAG, "Send rv=0x%x", rv);
	}
	if (rv == false) txLost++;
	return rv;
}


bool ReceiveMode(void)
{
	uint16_t irq;
	bool rv = false;

	if ( txActive == false )
	{
		rv = true;
	}
	else
	{
		irq = GetIrqStatus();
		if ( irq & (SX126X_IRQ_TX_DONE | SX126X_IRQ_TIMEOUT) )
		{ 
			SetRx(0xFFFFFF);
			txActive = false;
			rv = true;
		}
	}

	return rv;
}


void GetPacketStatus(int8_t *rssiPacket, int8_t *snrPacket)
{
	uint8_t buf[4];
	ReadCommand( SX126X_CMD_GET_PACKET_STATUS, buf, 4 ); // 0x14
	*rssiPacket = (buf[3] >> 1) * -1;
	( buf[2] < 128 ) ? ( *snrPacket = buf[2] >> 2 ) : ( *snrPacket = ( ( buf[2] - 256 ) >> 2 ) );
}


void SetTxPower(int8_t txPowerInDbm)
{
	SetPowerConfig(txPowerInDbm, SX126X_PA_RAMP_200U);
}


void Reset(void)
{
	delay(10);
	gpio_set_level(SX126x_RESET,0);
	delay(20);
	gpio_set_level(SX126x_RESET,1);
	delay(10);
	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "Reset", true);
}


void Wakeup(void)
{
	GetStatus();
}


void SetStandby(uint8_t mode)
{
	uint8_t data = mode;
	WriteCommand(SX126X_CMD_SET_STANDBY, &data, 1); // 0x80
}


uint8_t GetStatus(void)
{
	uint8_t rv;
	ReadCommand(SX126X_CMD_GET_STATUS, &rv, 1); // 0xC0
	return rv;
}


void SetDio3AsTcxoCtrl(float voltage, uint32_t delay)
{
	uint8_t buf[4];

	//buf[0] = tcxoVoltage & 0x07;
	if(fabs(voltage - 1.6) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_1_6;
	} else if(fabs(voltage - 1.7) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_1_7;
	} else if(fabs(voltage - 1.8) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_1_8;
	} else if(fabs(voltage - 2.2) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_2_2;
	} else if(fabs(voltage - 2.4) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_2_4;
	} else if(fabs(voltage - 2.7) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_2_7;
	} else if(fabs(voltage - 3.0) <= 0.001) {
		buf[0] = SX126X_DIO3_OUTPUT_3_0;
	} else {
		buf[0] = SX126X_DIO3_OUTPUT_3_3;
	}

	uint32_t delayValue = (float)delay / 15.625;
	buf[1] = ( uint8_t )( ( delayValue >> 16 ) & 0xFF );
	buf[2] = ( uint8_t )( ( delayValue >> 8 ) & 0xFF );
	buf[3] = ( uint8_t )( delayValue & 0xFF );

	WriteCommand(SX126X_CMD_SET_DIO3_AS_TCXO_CTRL, buf, 4); // 0x97
}


void Calibrate(uint8_t calibParam)
{
	uint8_t data = calibParam;
	WriteCommand(SX126X_CMD_CALIBRATE, &data, 1); // 0x89
}


void SetDio2AsRfSwitchCtrl(uint8_t enable)
{
	uint8_t data = enable;
	WriteCommand(SX126X_CMD_SET_DIO2_AS_RF_SWITCH_CTRL, &data, 1); // 0x9D
}


void SetRfFrequency(uint32_t frequency)
{
	uint8_t buf[4];
	uint32_t freq = 0;

	CalibrateImage(frequency);

	freq = (uint32_t)((double)frequency / (double)FREQ_STEP);
	buf[0] = (uint8_t)((freq >> 24) & 0xFF);
	buf[1] = (uint8_t)((freq >> 16) & 0xFF);
	buf[2] = (uint8_t)((freq >> 8) & 0xFF);
	buf[3] = (uint8_t)(freq & 0xFF);
	WriteCommand(SX126X_CMD_SET_RF_FREQUENCY, buf, 4); // 0x86
}


void CalibrateImage(uint32_t frequency)
{
	uint8_t calFreq[2];

	if( frequency> 900000000 )
	{
		calFreq[0] = 0xE1;
		calFreq[1] = 0xE9;
	}
	else if( frequency > 850000000 )
	{
		calFreq[0] = 0xD7;
		calFreq[1] = 0xDB;
	}
	else if( frequency > 770000000 )
	{
		calFreq[0] = 0xC1;
		calFreq[1] = 0xC5;
	}
	else if( frequency > 460000000 )
	{
		calFreq[0] = 0x75;
		calFreq[1] = 0x81;
	}
	else if( frequency > 425000000 )
	{
		calFreq[0] = 0x6B;
		calFreq[1] = 0x6F;
	}
	WriteCommand(SX126X_CMD_CALIBRATE_IMAGE, calFreq, 2); // 0x98
}


void SetRegulatorMode(uint8_t mode)
{
	uint8_t data = mode;
	WriteCommand(SX126X_CMD_SET_REGULATOR_MODE, &data, 1); // 0x96
}


void SetBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
	uint8_t buf[2];

	buf[0] = txBaseAddress;
	buf[1] = rxBaseAddress;
	WriteCommand(SX126X_CMD_SET_BUFFER_BASE_ADDRESS, buf, 2); // 0x8F
}


void SetPowerConfig(int8_t power, uint8_t rampTime)
{
	uint8_t buf[2];

	if( power > 22 )
	{
		power = 22;
	}
	else if( power < -3 )
	{
		power = -3;
	}
		
	buf[0] = power;
	buf[1] = ( uint8_t )rampTime;
	WriteCommand(SX126X_CMD_SET_TX_PARAMS, buf, 2); // 0x8E
}


void SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut)
{
	uint8_t buf[4];

	buf[0] = paDutyCycle;
	buf[1] = hpMax;
	buf[2] = deviceSel;
	buf[3] = paLut;
	WriteCommand(SX126X_CMD_SET_PA_CONFIG, buf, 4); // 0x95
}


void SetOvercurrentProtection(float currentLimit)
{
	if((currentLimit >= 0.0) && (currentLimit <= 140.0)) {
		uint8_t buf[1];
		buf[0] = (uint8_t)(currentLimit / 2.5);
		WriteRegister(SX126X_REG_OCP_CONFIGURATION, buf, 1); // 0x08E7
	}
}

void SetSyncWord(int16_t sync) {
	uint8_t buf[2];

	buf[0] = (uint8_t)((sync >> 8) & 0x00FF);
	buf[1] = (uint8_t)(sync & 0x00FF);
	WriteRegister(SX126X_REG_LORA_SYNC_WORD_MSB, buf, 2); // 0x0740
}

void SetDioIrqParams
( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
	uint8_t buf[8];

	buf[0] = (uint8_t)((irqMask >> 8) & 0x00FF);
	buf[1] = (uint8_t)(irqMask & 0x00FF);
	buf[2] = (uint8_t)((dio1Mask >> 8) & 0x00FF);
	buf[3] = (uint8_t)(dio1Mask & 0x00FF);
	buf[4] = (uint8_t)((dio2Mask >> 8) & 0x00FF);
	buf[5] = (uint8_t)(dio2Mask & 0x00FF);
	buf[6] = (uint8_t)((dio3Mask >> 8) & 0x00FF);
	buf[7] = (uint8_t)(dio3Mask & 0x00FF);
	WriteCommand(SX126X_CMD_SET_DIO_IRQ_PARAMS, buf, 8); // 0x08
}


void SetStopRxTimerOnPreambleDetect(bool enable)
{
	ESP_LOGI(TAG, "SetStopRxTimerOnPreambleDetect enable=%d", enable);
	//uint8_t data = (uint8_t)enable;
	uint8_t data = 0;
	if (enable) data = 1;
	WriteCommand(SX126X_CMD_STOP_TIMER_ON_PREAMBLE, &data, 1); // 0x9F
}


void SetLoRaSymbNumTimeout(uint8_t SymbNum)
{
	uint8_t data = SymbNum;
	WriteCommand(SX126X_CMD_SET_LORA_SYMB_NUM_TIMEOUT, &data, 1); // 0xA0
}


void SetPacketType(uint8_t packetType)
{
	uint8_t data = packetType;
	WriteCommand(SX126X_CMD_SET_PACKET_TYPE, &data, 1); // 0x01
}


void SetModulationParams(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint8_t lowDataRateOptimize)
{
	uint8_t data[4];
	//currently only LoRa supported
	data[0] = spreadingFactor;
	data[1] = bandwidth;
	data[2] = codingRate;
	data[3] = lowDataRateOptimize;
	WriteCommand(SX126X_CMD_SET_MODULATION_PARAMS, data, 4); // 0x8B
}


void SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, uint8_t cadExitMode, uint32_t cadTimeout)
{
	uint8_t data[7];
	data[0] = cadSymbolNum;
	data[1] = cadDetPeak;
	data[2] = cadDetMin;
	data[3] = cadExitMode;
	data[4] = (uint8_t)((cadTimeout >> 16) & 0xFF);
	data[5] = (uint8_t)((cadTimeout >> 8) & 0xFF);
	data[6] = (uint8_t)(cadTimeout & 0xFF);
	WriteCommand(SX126X_CMD_SET_CAD_PARAMS, data, 7); // 0x88
}


void SetCad()
{
	uint8_t data = 0;
	WriteCommand(SX126X_CMD_SET_CAD, &data, 0); // 0xC5
}


uint16_t GetIrqStatus( void )
{
	uint8_t data[3];
	ReadCommand(SX126X_CMD_GET_IRQ_STATUS, data, 3); // 0x12
	return (data[1] << 8) | data[2];
}


void ClearIrqStatus(uint16_t irq)
{
	uint8_t buf[2];

	buf[0] = (uint8_t)(((uint16_t)irq >> 8) & 0x00FF);
	buf[1] = (uint8_t)((uint16_t)irq & 0x00FF);
	WriteCommand(SX126X_CMD_CLEAR_IRQ_STATUS, buf, 2); // 0x02
}


void SetRx(uint32_t timeout)
{
	if (debugPrint) {
		ESP_LOGI(TAG, "----- SetRx timeout=%"PRIu32, timeout);
	}
	SetStandby(SX126X_STANDBY_RC);
	SetRxEnable();
	uint8_t buf[3];
	buf[0] = (uint8_t)((timeout >> 16) & 0xFF);
	buf[1] = (uint8_t)((timeout >> 8) & 0xFF);
	buf[2] = (uint8_t)(timeout & 0xFF);
	WriteCommand(SX126X_CMD_SET_RX, buf, 3); // 0x82

	for(int retry=0;retry<10;retry++) {
		if ((GetStatus() & 0x70) == 0x50) break;
		delay(1);
	}
	if ((GetStatus() & 0x70) != 0x50) {
		ESP_LOGE(TAG, "SetRx Illegal Status");
		LoRaError(ERR_INVALID_SETRX_STATE);
	}
}


void SetRxEnable(void)
{
	if (debugPrint) {
		ESP_LOGI(TAG, "SetRxEnable:SX126x_TXEN=%d SX126x_RXEN=%d", SX126x_TXEN, SX126x_RXEN);
	}
	if ((SX126x_TXEN != -1) && (SX126x_RXEN != -1)) {
		gpio_set_level(SX126x_RXEN, HIGH);
		gpio_set_level(SX126x_TXEN, LOW);
	}
}


void SetTx(uint32_t timeoutInMs)
{
	if (debugPrint) {
		ESP_LOGI(TAG, "----- SetTx timeoutInMs=%"PRIu32, timeoutInMs);
	}
	SetStandby(SX126X_STANDBY_RC);
	SetTxEnable();
	uint8_t buf[3];
	uint32_t tout = timeoutInMs;
	if (timeoutInMs != 0) {
		uint32_t timeoutInUs = timeoutInMs * 1000;
		tout = (uint32_t)(timeoutInUs / 0.015625);
	}
	if (debugPrint) {
		ESP_LOGI(TAG, "SetTx timeoutInMs=%"PRIu32" tout=%"PRIu32, timeoutInMs, tout);
	}
	buf[0] = (uint8_t)((tout >> 16) & 0xFF);
	buf[1] = (uint8_t)((tout >> 8) & 0xFF);
	buf[2] = (uint8_t )(tout & 0xFF);
	WriteCommand(SX126X_CMD_SET_TX, buf, 3); // 0x83
	
	for(int retry=0;retry<10;retry++) {
		if ((GetStatus() & 0x70) == 0x60) break;
		vTaskDelay(1);
	}
	if ((GetStatus() & 0x70) != 0x60) {
		ESP_LOGE(TAG, "SetTx Illegal Status");
		LoRaError(ERR_INVALID_SETTX_STATE);
	}
}


void SetTxEnable(void)
{
	if (debugPrint) {
		ESP_LOGI(TAG, "SetTxEnable:SX126x_TXEN=%d SX126x_RXEN=%d", SX126x_TXEN, SX126x_RXEN);
	}
	if ((SX126x_TXEN != -1) && (SX126x_RXEN != -1)){
		gpio_set_level(SX126x_RXEN, LOW);
		gpio_set_level(SX126x_TXEN, HIGH);
	}
}


int GetPacketLost()
{
	return txLost;
}


uint8_t GetRssiInst()
{
	uint8_t buf[2];
	ReadCommand( SX126X_CMD_GET_RSSI_INST, buf, 2 ); // 0x15
	return buf[1];
}


void GetRxBufferStatus(uint8_t *payloadLength, uint8_t *rxStartBufferPointer)
{
	uint8_t buf[3];
	ReadCommand( SX126X_CMD_GET_RX_BUFFER_STATUS, buf, 3 ); // 0x13
	*payloadLength = buf[1];
	*rxStartBufferPointer = buf[2];
}


void WaitForIdleBegin(unsigned long timeout, char *text) {
	// ensure BUSY is low (state meachine ready)
	bool stop = false;
	for (int retry=0;retry<10;retry++) {
		if (retry == 9) stop = true;
		bool ret = WaitForIdle(BUSY_WAIT, text, stop);
		if (ret == true) break;
		ESP_LOGW(TAG, "WaitForIdle fail retry=%d", retry);
		vTaskDelay(1);
	}
}


bool WaitForIdle(unsigned long timeout, char *text, bool stop)
{
	bool ret = true;
	TickType_t start = xTaskGetTickCount();
	delayMicroseconds(1);
	while(xTaskGetTickCount() - start < (timeout/portTICK_PERIOD_MS)) {
		if (gpio_get_level(SX126x_BUSY) == 0) break;
		delayMicroseconds(1);
	}
	if (gpio_get_level(SX126x_BUSY)) {
		if (stop) {
			ESP_LOGE(TAG, "WaitForIdle Timeout text=%s timeout=%lu start=%"PRIu32, text, timeout, start);
			LoRaError(ERR_IDLE_TIMEOUT);
		} else {
			ESP_LOGW(TAG, "WaitForIdle Timeout text=%s timeout=%lu start=%"PRIu32, text, timeout, start);
			ret = false;
		}
	}
	return ret;
}


uint8_t ReadBuffer(uint8_t *rxData, int16_t rxDataLen)
{
	uint8_t offset = 0;
	uint8_t payloadLength = 0;
	GetRxBufferStatus(&payloadLength, &offset);
	if( payloadLength > rxDataLen )
	{
		ESP_LOGW(TAG, "ReadBuffer rxDataLen too small. payloadLength=%d rxDataLen=%d", payloadLength, rxDataLen);
		return 0;
	}

	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "start ReadBuffer", true);

	// start transfer
	uint8_t *buf;
	buf = malloc(payloadLength+3);
	if (buf != NULL) {
		buf[0] = SX126X_CMD_READ_BUFFER; // 0x1E
		buf[1] = offset; // offset in rx fifo
		buf[2] = SX126X_CMD_NOP;
		memset(&buf[3], SX126X_CMD_NOP, payloadLength);
		spi_read_byte(buf, buf, payloadLength+3);
		memcpy(rxData, &buf[3], payloadLength);
		free(buf);
	} else {
		ESP_LOGE(TAG, "ReadBuffer malloc fail");
		payloadLength = 0;
	}

	// wait for BUSY to go low
	WaitForIdle(BUSY_WAIT, "end ReadBuffer", false);

	return payloadLength;
}


void WriteBuffer(uint8_t *txData, int16_t txDataLen)
{
	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "start WriteBuffer", true);

	// start transfer
	uint8_t *buf;
	buf = malloc(txDataLen+2);
	if (buf != NULL) {
		buf[0] = SX126X_CMD_WRITE_BUFFER; // 0x0E
		buf[1] = 0; // offset in tx fifo
		memcpy(&buf[2], txData, txDataLen);
		spi_write_byte(buf, txDataLen+2);
		free(buf);
	} else {
		ESP_LOGE(TAG, "WriteBuffer malloc fail");
	}

	// wait for BUSY to go low
	WaitForIdle(BUSY_WAIT, "end WriteBuffer", false);
}


void WriteRegister(uint16_t reg, uint8_t* data, uint8_t numBytes) {
	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "start WriteRegister", true);

	if(debugPrint) {
		ESP_LOGI(TAG, "WriteRegister: REG=0x%02x", reg);
		for(uint8_t n = 0; n < numBytes; n++) {
			ESP_LOGI(TAG, "DataOut:%02x ", data[n]);
		}
	}

	// start transfer
	uint8_t buf[16];
	buf[0] = SX126X_CMD_WRITE_REGISTER;
	buf[1] = (reg & 0xFF00) >> 8;
	buf[2] = reg & 0xff;
	memcpy(&buf[3], data, numBytes);
	spi_write_byte(buf, 3 + numBytes);

	// wait for BUSY to go low
	WaitForIdle(BUSY_WAIT, "end WriteRegister", false);
}


void ReadRegister(uint16_t reg, uint8_t* data, uint8_t numBytes) {
	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "start ReadRegister", true);

	if(debugPrint) {
		ESP_LOGI(TAG, "ReadRegister: REG=0x%02x", reg);
	}

	// start transfer
	uint8_t buf[16];
	memset(buf, SX126X_CMD_NOP, sizeof(buf));
	buf[0] = SX126X_CMD_READ_REGISTER;
	buf[1] = (reg & 0xFF00) >> 8;
	buf[2] = reg & 0xff;
	spi_read_byte(buf, buf, 4 + numBytes);
	memcpy(data, &buf[4], numBytes);
	if(debugPrint) {
		for(uint8_t n = 0; n < numBytes; n++) {
			ESP_LOGI(TAG, "DataIn:%02x ", data[n]);
		}
	}

	// wait for BUSY to go low
	WaitForIdle(BUSY_WAIT, "end ReadRegister", false);
}

// WriteCommand with retry
void WriteCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes) {
	uint8_t status;
	for (int retry=1; retry<10; retry++) {
		status = WriteCommand2(cmd, data, numBytes);
		ESP_LOGD(TAG, "status=%02x", status);
		if (status == 0) break;
		ESP_LOGW(TAG, "WriteCommand2 status=%02x retry=%d", status, retry);
	}
	if (status != 0) {
		ESP_LOGE(TAG, "SPI Transaction error:0x%02x", status);
		LoRaError(ERR_SPI_TRANSACTION);
	}
}

uint8_t WriteCommand2(uint8_t cmd, uint8_t* data, uint8_t numBytes) {
	// ensure BUSY is low (state meachine ready)
	WaitForIdle(BUSY_WAIT, "start WriteCommand2", true);

	if(debugPrint) {
		ESP_LOGI(TAG, "WriteCommand: CMD=0x%02x", cmd);
	}

	// start transfer
	uint8_t buf[16];
	buf[0] = cmd;
	memcpy(&buf[1], data, numBytes);
	spi_read_byte(buf, buf, numBytes + 1);

	uint8_t status = 0;
	uint8_t cmd_status = buf[1] & 0xe;

	switch(cmd_status){
		case SX126X_STATUS_CMD_TIMEOUT:
		case SX126X_STATUS_CMD_INVALID:
		case SX126X_STATUS_CMD_FAILED:
			status = cmd_status;
			break;

		case 0:
		case 7:
			status = SX126X_STATUS_SPI_FAILED;
			break;
			// default: break; // success
	}

	// wait for BUSY to go low
	WaitForIdle(BUSY_WAIT, "end WriteCommand2", false);
	return status;
}


void ReadCommand(uint8_t cmd, uint8_t* data, uint8_t numBytes) {
	// ensure BUSY is low (state meachine ready)
	WaitForIdleBegin(BUSY_WAIT, "start ReadCommand");

	if(debugPrint) {
		ESP_LOGI(TAG, "ReadCommand: CMD=0x%02x", cmd);
	}

	// start transfer
	uint8_t buf[16];
	memset(buf, SX126X_CMD_NOP, sizeof(buf));
	buf[0] = cmd;
	spi_read_byte(buf, buf, 1 + numBytes);
	if (data != NULL && numBytes)
		memcpy(data, &buf[1], numBytes);

	// wait for BUSY to go low
	vTaskDelay(1);
	WaitForIdle(BUSY_WAIT, "end ReadCommand", false);
}
