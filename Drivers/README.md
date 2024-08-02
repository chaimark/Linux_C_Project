- ## FM33LC0xx FL驱动库改版记录
  
  ### v2.0.0
  
  - 首次发布
  
  ### v2.0.1
  
  - **fm33lc0xx_fl.h**：FL_DELAY_US和FL_DELAY_MS使用SystemCoreClock实现，用户在修改主频后可以直接调用SystemCoreClockUpdate()函数让FL库延时保持准确。
  - **fm33lc0xx_fl_flash.c**：将函数中FL_FLASH_SECTOR_SIZE_BYTE修改为FL_FLASH_PAGE_SIZE_BYTE，修正传输长度问题。
  - **fm33lc0xx_fl_uart.h，fm33lc0xx_fl_lpuart.h**：修改UART和LPUART的读写TXBUF/RXBUF以及LPUART的读写DMR的FL库函数的操作位宽为9位以支持9bit UART通讯。
  
  ### v2.1.0
  
  - **fm33lc0xx_fl_atim.c，fm33lc0xx_fl_atim.h**：ATIM输出比较初始化结构体FL_ATIM_OC_InitTypeDef中增加正向通道输出控制配置项。（该更新不向前兼容）
  
  ### v2.1.1
  
  - **CMSIS**：添加了fm33_assert.h头文件，解除FL驱动库和工程文件的耦合。
  - **fm33lc0xx_fl.h**：添加对fm33_assert.h头文件的显式包含。
  - **fm33lc0xx_fl_lcd.h**：修正FL_LCD_SEGEN0_SEG3宏定义的位宽错误。
  - **fm33lc0xx_fl_rng.c**：增加两个随机数的时间，从10us增加到12us，使得在3MHz（典型情况为24M使用8分频）工作时钟的情况下随机数质量也能得到保证。
  
  ### v2.2.0
  
  - **fm33lc0xx_lpuart.c**：修改LPUART初始化函数，如果用户选择工作时钟为RCMF，则初始化函数中使能RCMF。
  - **fm33lc0xx_rcc.c/.h**：增加USB BCK时钟配置函数，调整相关函数名称使其符合实际意义；修复FL_RCC_XTHF_ReadDriverStrength函数读取位段错误；统一修改部分宏定义中使用的“RCLP”为“LPOSC”，RCMFPSC改为RCMF_PSC。（该更新不向前兼容）
  - **fm33lc0xx_fl_exti.c**：增加头文件包含，解决在魔方生成的代码中编译报错的问题。
  - **fm33lc0xx_fl_rmu.h**：修正RMU_BORCR_CFG_Pos宏定义错误。
  - 更新了开源许可证协议为**Mulan PSL V2**
  
  ### v2.3.0
  
  - 文件变动：
    - 将**fm33_assert.h**文件从CMSIS对应目录移动到FL驱动库对应目录下；
    - 添加了**fm33lc0xx_fl_conf.h**和**fm33lc0xx_fl_def.h**，删除了**fm33xx.h**，将内容移动到**fm33lc0xx_fl_def.h**中；
    - 将**fm33lc0xx_fl.h**中的“包含外设头文件”部分的内容移动到了**fm33lc0xx_fl_conf.h**, 并在**fm33lc0xx_fl.h**中包含它。
    - 将**fm33lc0xx_fl.h**中的“FL驱动库返回类型定义”内容移动到了**fm33lc0xx_fl_def.h**, 并在**fm33lc0xx_fl.h**中包含它。
    - 将所有外设驱动.c文件的包含头文件替换为**fm33lc0xx_fl.h**，并使用FL_XXX_DRIVER_ENABLED条件编译包裹；
    - 将所有外设驱动.h文件的包含头文件替换为**fm33lc0xx_def.h**；
    - 取消使用#ifndef MFANG条件编译包裹外设的FL_XXX_DRIVER_ENABLED列表，防止冲突；
  - **CMSIS**：
    - **system_fm33lc0xx.c/.h**: 
      - 删除原先的XTHF_VALUE和XTLF_VALUE，取而代之定义了XTHFClock和XTLFClock全局变量。如果用户使用XTHF或者XTLF，需要在FL库初始化前为该两个变量重新赋予正确的值并调用SystemCoreClockUpdate()函数以正确初始化FL库延时函数；
      - 为SystemInit函数中的时钟配置部分使用#ifndef MFANG条件编译包裹起来，使得魔方能够掌控时钟初始化内容；
      - 添加**USE_IWDT_ON_STARTUP**和**IWDT_OVERFLOW_PERIOD**宏定义，如果配置为使能，则在SystemInit函数中就初始化IWDT，保证芯片初始化时的抗干扰能力；
  - **fm33lc0xx_fl_rcc.c**：修改获取时钟频率函数以适应CMSIS的修改；
  - **fm33lc0xx_fl_exti.c**：修改外部中断初始化延时，满足手册大于3个LSCLK的周期的要求；
  - **fm33lc0xx_fl_gpio.h**：修正FOUT选择宏定义中的FL_GPIO_FOUTx_SELECT_RCLP为FL_GPIO_FOUTx_SELECT_LPOSC，FL_GPIO_FOUTx_SELECT_APBCLK_DIV64为FL_GPIO_FOUTx_SELECT_APB1CLK_DIV64；修改FOUT0和FOUT1的宏定义位置错位问题；
  - **fm33lc0xx_fl_gpio.h, fm33lc0xx_fl_rcc.h, fm33lc0xx_fl_i2c.c, fm33lc0xx_fl_rcc.c, fm33lc0xx_fl_uart.c**：修改RC4M为RCMF；
  - **fm33lc0xx_fl_adc.h**：添加FL_ADC_ALL_CHANNEL宏定义，便于一次性关闭所有ADC通道。

### V2.3.1

- **CMSIS**:
  
  - **fm33lc0xx.h**：修正USB结构体寄存器偏移。
  
  - **system_fm33lc0xx.c/.h**：添加**USE_LSCLK_AUTO_SWITCH**宏定义用于在LSCLK使用XTLF（即打开**USE_LSCLK_CLOCK_SRC_XTLF**宏定义）时配置自动切换的使能。

- **fm33lc0xx_fl_gpio.h**, **fm33lc0xx_fl_spi.h**: 优化部分函数执行速度。

- **fm33lc0xx_fl_atim.c, fm33lc0xx_fl_bstim32.c, fm33lc0xx_fl_gptim.c**：修正外设初始化函数等待Update标志时错误地使用&判断的问题，改用&&。【重要】
