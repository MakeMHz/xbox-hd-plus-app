XBE_TITLE = XboxHD+
GEN_XISO = $(XBE_TITLE).iso
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y

#Include my files
SRCS += \
	$(wildcard $(CURDIR)/src/*.cpp) \
	$(wildcard $(CURDIR)/src/Generated/*.cpp) \
	$(wildcard $(CURDIR)/src/lvgl-extra/*.cpp) \
	$(wildcard $(CURDIR)/src/lvgl-style/*.c) \
	$(wildcard $(CURDIR)/src/scenes/*.cpp)
# lwip
SRCS += \
	$(CURDIR)/src/lwip/http_client.c
# Assets
SRCS += \
	$(wildcard $(CURDIR)/assets/*.c)
CFLAGS += \
	-I$(CURDIR)/include \
	-I$(CURDIR)/src \
	-DLWIP_HTTPC_HAVE_FILE_IO \
	-DSDL_DISABLE_JOYSTICK_INIT_DELAY \
	-O2
CXXFLAGS += \
	-I$(CURDIR)/include \
	-I$(CURDIR)/src \
	-DLWIP_HTTPC_HAVE_FILE_IO \
	-DSDL_DISABLE_JOYSTICK_INIT_DELAY \
	-O2

#Include lvgl main library
SRCS +=	\
	$(CURDIR)/3rdparty/lvgl-sdl/lv_drv/lv_sdl_drv_input.c \
	$(CURDIR)/3rdparty/lvgl-sdl/lv_drv/lv_sdl_drv_display.c \
	$(CURDIR)/3rdparty/lvgl-sdl/lv_drv/lv_if_drv_filesystem.c \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_core/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_draw/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_font/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_gpu/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_hal/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_misc/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_themes/*.c) \
	$(wildcard $(CURDIR)/3rdparty/lvgl-sdl/lvgl/src/lv_widgets/*.c)
CFLAGS += \
	-I$(CURDIR)/3rdparty/lvgl-sdl/ \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lv_drv \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lvgl/ \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lvgl/src
CXXFLAGS += \
	-I$(CURDIR)/3rdparty/lvgl-sdl/ \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lv_drv \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lvgl/ \
	-I$(CURDIR)/3rdparty/lvgl-sdl/lvgl/src

# JSON
CXXFLAGS += \
	-I$(CURDIR)/3rdparty/json/single_include/nlohmann/

# semver.c
SRCS +=	\
	$(CURDIR)/3rdparty/semver.c/semver.c
CXXFLAGS += \
	-I$(CURDIR)/3rdparty/semver.c

include $(NXDK_DIR)/Makefile
