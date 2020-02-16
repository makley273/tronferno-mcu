
ifeq ($(BOOT), new)
    boot = new
else
    ifeq ($(BOOT), old)
        boot = old
    else
        boot = none
    endif
endif

ifeq ($(APP), 1)
    app = 1
else
    ifeq ($(APP), 2)
        app = 2
    else
        app = 0
    endif
endif

ifeq ($(SPI_SPEED), 26.7)
    freqdiv = 1
    flashimageoptions = -ff 26m
else
    ifeq ($(SPI_SPEED), 20)
        freqdiv = 2
        flashimageoptions = -ff 20m
    else
        ifeq ($(SPI_SPEED), 80)
            freqdiv = 15
            flashimageoptions = -ff 80m
        else
            freqdiv = 0
            flashimageoptions = -ff 40m
        endif
    endif
endif

ifeq ($(SPI_MODE), QOUT)
    mode = 1
    flashimageoptions += -fm qout
else
    ifeq ($(SPI_MODE), DIO)
	mode = 2
	flashimageoptions += -fm dio
    else
        ifeq ($(SPI_MODE), DOUT)
            mode = 3
            flashimageoptions += -fm dout
        else
            mode = 0
            flashimageoptions += -fm qio
        endif
    endif
endif

addr = 0x01000

ifeq ($(SPI_SIZE_MAP), 1)
  size_map = 1
  flash = 256
  flashimageoptions += -fs 256KB
else
  ifeq ($(SPI_SIZE_MAP), 2)
    size_map = 2
    flash = 1024
    flashimageoptions += -fs 1MB
    ifeq ($(app), 2)
      addr = 0x81000
    endif
  else
    ifeq ($(SPI_SIZE_MAP), 3)
      size_map = 3
      flash = 2048
      flashimageoptions += -fs 2MB
      ifeq ($(app), 2)
        addr = 0x81000
      endif
    else
      ifeq ($(SPI_SIZE_MAP), 4)
	size_map = 4
	flash = 4096
	flashimageoptions += -fs 4MB
        ifeq ($(app), 2)
          addr = 0x81000
        endif
      else
        ifeq ($(SPI_SIZE_MAP), 5)
          size_map = 5
          flash = 2048
          flashimageoptions += -fs 2MB-c1
          ifeq ($(app), 2)
            addr = 0x101000
          endif
        else
          ifeq ($(SPI_SIZE_MAP), 6)
            size_map = 6
            flash = 4096
            flashimageoptions += -fs 4MB-c1
            ifeq ($(app), 2)
              addr = 0x101000
            endif
          else
            size_map = 0
            flash = 512
            flashimageoptions += -fs 512KB
            ifeq ($(app), 2)
              addr = 0x41000
            endif
          endif
        endif
      endif
    endif
  endif
endif


D ?= $(DEBUG)
ifeq ("$(D)","1")

endif

EXTRA_INCDIR   += $(EXTRA_BASE)/include

# compiler flags using during compilation of source files
CFLAGS		=  -std=gnu90 -Wpointer-arith -Wundef -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -mno-serialize-volatile -D__ets__ -DICACHE_FLASH
CXXFLAGS	= -Os -g -O2  -Wpointer-arith -Wundef -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -mno-serialize-volatile -D__ets__ -DICACHE_FLASH -fno-rtti -fno-exceptions

# linker flags used to generate the main object file
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static



# linker script used for the above linkier step
LD_SCRIPT	= eagle.app.v6.ld

ifneq ($(boot), none)
ifneq ($(app),0)
    ifeq ($(size_map), 6)
      LD_SCRIPT = eagle.app.v6.$(boot).2048.ld
    else
      ifeq ($(size_map), 5)
        LD_SCRIPT = eagle.app.v6.$(boot).2048.ld
      else
        ifeq ($(size_map), 4)
          LD_SCRIPT = eagle.app.v6.$(boot).1024.app$(app).ld
        else
          ifeq ($(size_map), 3)
            LD_SCRIPT = eagle.app.v6.$(boot).1024.app$(app).ld
          else
            ifeq ($(size_map), 2)
              LD_SCRIPT = eagle.app.v6.$(boot).1024.app$(app).ld
            else
              ifeq ($(size_map), 0)
                LD_SCRIPT = eagle.app.v6.$(boot).512.app$(app).ld
              endif
            endif
	      endif
	    endif
	  endif
	endif
	BIN_NAME = user$(app).$(flash).$(boot).$(size_map)
	CFLAGS += -DAT_UPGRADE_SUPPORT
endif
else
    app = 0
endif

ifndef SRC_BASE
SRC_BASE := .
endif

CPPFLAGS += -DSPI_FLASH_SIZE_MAP=$(size_map)

DEP_DIR := $(BUILD_BASE)/dep

# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SDK_INCDIR	= include include/json



# select which tools to use as compiler, librarian and linker
CC		:= xtensa-lx106-elf-gcc 
CXX		:= xtensa-lx106-elf-g++
AR		:= xtensa-lx106-elf-ar
LD		:= xtensa-lx106-elf-gcc
OBJCOPY		:= xtensa-lx106-elf-objcopy
OBJDUMP		:= xtensa-lx106-elf-objdump

SRC_DIR		:= $(addprefix $(SRC_BASE)/,$(MODULES))
BUILD_DIRS	+= $(addprefix $(BUILD_BASE)/,$(MODULES))
SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))

C_VERSION_SRC := $(SRC_BASE)/src/build_date.c
C_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
CXX_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
ASM_SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.S))

C_VERSION_OBJ   := $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(C_VERSION_SRC)))
C_OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(C_SRC)))
CXX_OBJ		:= $(patsubst %.cpp,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(CXX_SRC)))
ASM_OBJ	    	:= $(patsubst %.S,$(BUILD_BASE)/%.o,$(subst $(SRC_BASE)/,,$(ASM_SRC)))

SRC		:= $(C_SRC) $(CXX_SRC) $(ASM_SRC)
OBJ		:= $(C_OBJ) $(CXX_OBJ) $(ASM_OBJ)

LIBS		:= $(addprefix -l,$(LIBS))
APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)

LD_SCRIPT	:= $(addprefix -T$(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))

INCDIR		:= -Isrc -I$(SRC_BASE)/src/time -I$(SRC_BASE)/src # $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(EXTRA_INCDIR))
#MODULE_INCDIR	:= $(foreach sdir,$(SRC_DIR),$(addprefix -I,$(sdir)))

ifeq ($(DISTRO),1)
CPPFLAGS += -DDISTRIBUTION
endif

ifneq ("$(D)","1")
CFLAGS += -Os -g -O2
endif

ifneq ($(wildcard $(SRC_BASE)/src/sensitive/defaults.h),)
CPPFLAGS += -DHAVE_USER_DEFAULTS
endif

pri:
	echo $(OBJ)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

define compile-objects
$(1)%.o: $(2)%.S
	$(vecho) "AS $$<"
	$(Q) $(CC) $(CPPFLAGS) -c $$< -o $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CC) $(CPPFLAGS) -c $$< -o $$@
$(1)%.o: $(2)%.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS) -c $$< -MT $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CFLAGS)  -c $$< -o $$@
$(1)%.o: $(2)%.cpp
	$(vecho) "C+ $$<"
	$(Q) $(CXX) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CXXFLAGS) -c $$< -MT $$@ -MM > $$(patsubst %.o,%.d,$(DEP_DIR)/$$(subst /,-,$$@))
	$(Q) $(CXX) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CPPFLAGS) $(CXXFLAGS)  -c $$< -o $$@
endef


.PHONY: all checkdirs clean flash flashboot flashinit flasherase rebuild reflash 

all: checkdirs $(TARGET_OUT) $(SUBMODULES)

$(TARGET_OUT): $(APP_AR) $(GEN_LIBS)
	$(MAKE) $(C_VERSION_OBJ)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) -L$(BUILD_BASE) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group  $(LIBS) $(APP_AR) $(EXTRA_AR) $(C_VERSION_OBJ) -Wl,--end-group -o $@
	$(vecho) "Run objcopy, please wait..."
	$(Q) $(OBJCOPY) --only-section .text -O binary $@ eagle.app.v6.text.bin
	$(Q) $(OBJCOPY) --only-section .data -O binary $@ eagle.app.v6.data.bin
	$(Q) $(OBJCOPY) --only-section .rodata -O binary $@ eagle.app.v6.rodata.bin
	$(Q) $(OBJCOPY) --only-section .irom0.text -O binary $@ eagle.app.v6.irom0text.bin
	$(vecho) "objcopy done"
	$(vecho) "Run gen_appbin"
ifeq ($(app), 0)
	$(Q) python $(SDK_TOOLS)/gen_appbin.py $@ 0 $(mode) $(freqdiv) $(size_map) $(app)
	$(Q) mv eagle.app.flash.bin $(FW_BASE)/eagle.flash.bin
	$(Q) mv eagle.app.v6.irom0text.bin $(FW_BASE)/eagle.irom0text.bin
	$(Q) rm eagle.app.v6.*
	$(vecho) "No boot needed."
	$(vecho) "Generate eagle.flash.bin and eagle.irom0text.bin successully in folder $(FW_BASE)"
	$(vecho) "eagle.flash.bin-------->0x00000"
	$(vecho) "eagle.irom0text.bin---->0x10000"
else
    ifneq ($(boot), new)
	$(Q)  python $(SDK_TOOLS)/gen_appbin.py $@ 1 $(mode) $(freqdiv) $(size_map) $(app)
	$(vecho) "Support boot_v1.1 and +"
    else
	$(Q)  python $(SDK_TOOLS)/gen_appbin.py $@ 2 $(mode) $(freqdiv) $(size_map) $(app)
        ifeq ($(size_map), 6)
		$(vecho) "Support boot_v1.4 and +"
        else
            ifeq ($(size_map), 5)
		$(vecho) "Support boot_v1.4 and +"
            else
		$(vecho) "Support boot_v1.2 and +"
            endif
        endif
    endif
	$(Q) mv eagle.app.flash.bin $(FW_BASE)/upgrade/$(BIN_NAME).bin
	$(Q) rm eagle.app.v6.*
	$(vecho) "Generate $(BIN_NAME).bin successully in folder $(FW_BASE)/upgrade"
	$(vecho) "boot.bin------->0x00000"
	$(vecho) "$(BIN_NAME).bin--->$(addr)"
endif
	$(vecho) "Done"

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@.tmp $^
	$(Q) $(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal --rename-section .iram0.text=.text $@.tmp $@


checkdirs: $(BUILD_DIRS) $(FW_BASE) $(DEP_DIR)

$(BUILD_DIRS):
	$(vecho) mkdir $@
	$(Q) mkdir -p $@

$(DEP_DIR):
	$(Q) mkdir -p $@

$(FW_BASE):
	$(Q) mkdir -p $@
	$(Q) mkdir -p $@/upgrade


flashboot:
ifeq ($(app), 0)
	$(vecho) "No boot needed."
else
    ifneq ($(boot), new)
	$(vecho) "Flash boot_v1.1 and +"
	$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(SDK_BASE)/bin/boot_v1.1.bin
    else
        ifeq ($(size_map), 6)
		$(vecho) "Flash boot_v1.5 and +"
		$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(SDK_BASE)/bin/boot_v1.6.bin
        else
            ifeq ($(size_map), 5)
		$(vecho) "Flash boot_v1.5 and +"
		$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(SDK_BASE)/bin/boot_v1.6.bin
            else
		$(vecho) "Flash boot_v1.2 and +"
		$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(SDK_BASE)/bin/boot_v1.2.bin
            endif
        endif
    endif
endif

flash: all
ifeq ($(app), 0) 
	$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(FW_BASE)/eagle.flash.bin 0x10000 $(FW_BASE)/eagle.irom0text.bin
else
ifeq ($(boot), none)
	$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) 0x00000 $(FW_BASE)/eagle.flash.bin 0x10000 $(FW_BASE)/eagle.irom0text.bin
else
	$(ESPTOOL) -p $(ESPPORT) -b $(ESPBAUD) write_flash $(flashimageoptions) $(addr) $(FW_BASE)/upgrade/$(BIN_NAME).bin
endif
endif

# ===============================================================
# From http://bbs.espressif.com/viewtopic.php?f=10&t=305
# master-device-key.bin is only need if using espressive services
# master_device_key.bin 0x3e000 is not used , write blank
# See 2A-ESP8266__IOT_SDK_User_Manual__EN_v1.1.0.pdf
# http://bbs.espressif.com/download/file.php?id=532
#
# System parameter area is the last 16KB of flash
# 512KB flash - system parameter area starts from 0x7C000 
# 	download blank.bin to 0x7E000 as initialization.
# 1024KB flash - system parameter area starts from 0xFC000 
# 	download blank.bin to 0xFE000 as initialization.
# 2048KB flash - system parameter area starts from 0x1FC000 
# 	download blank.bin to 0x1FE000 as initialization.
# 4096KB flash - system parameter area starts from 0x3FC000 
# 	download blank.bin to 0x3FE000 as initialization.
# ===============================================================

# FLASH SIZE


ifeq ($(flash), 512)
 sparea = 0x7C000
 bbarea = 0x7E000
else
 ifeq ($(flash), 1024)
   sparea = 0xFC000
   bbarea = 0xFE000
 else
   ifeq ($(flash), 2048)
     sparea = 0x1FC000
     bbarea = 0x1FE000
   else
     ifeq ($(flash), 4096)
       sparea = 0x3FC000
       bbarea = 0x3FE000
     endif
   endif
  endif
endif


flashinit:
	$(vecho) "Flash init data default and blank data."
	$(ESPTOOL) -p $(ESPPORT) write_flash $(flashimageoptions) $(sparea) $(SDK_BASE)/bin/esp_init_data_default_v08.bin $(bbarea) $(SDK_BASE)/bin/blank.bin

flasherase:
	$(vecho) "Flash erase. May be followed by: make flashinit"
	$(ESPTOOL) -p $(ESPPORT) erase_flash


rebuild: clean all

reflash: rebuild flash


clean: force
	$(Q) rm -f $(APP_AR)
	$(Q) rm -f $(EXTRA_AR)
	$(Q) rm -f $(TARGET_OUT)
	$(Q) rm -rf $(BUILD_DIRS)
	$(Q) rm -rf $(DEP_DIR)
	$(Q) rm -rf $(FW_BASE)
	$(Q) rm -rf $(BUILD_BASE)/src $(BUILD_BASE)
	$(Q) rm -rf spiffs/build/*.o  spiffs/build/*.a
	$(Q) mkdir -p $(DEP_DIR) # FIXME: kludge to allow "make -j" 


$(C_VERSION_SRC) : force
	echo 'const char * const build_time = "'$$(date --iso-8601=seconds)'";' > $(C_VERSION_SRC)


$(eval $(call compile-objects,$(BUILD_BASE)/,$(SRC_BASE)/))

include $(wildcard $(DEP_DIR)/*.d) $(wildcard $(DEP_DIR)/.*.d)
