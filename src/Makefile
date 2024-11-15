#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)CAN4CQF
#
# This file was generated with the command:
#  opp_makemake --make-so -f --deep -KCORE4INET_PROJ=../../CoRE4INET -KINET_PROJ=../../inet -KFICO4OMNET_PROJ=../../FiCo4OMNeT -KSIGNALSANDGATEWAYS_PROJ=../../SignalsAndGateways -DINET_IMPORT -I. -I$$\(CORE4INET_PROJ\)/src -I$$\(CORE4INET_PROJ\)/src/core4inet/api/AS6802/tte_api -I$$\(INET_PROJ\)/src -I$$\(FICO4OMNET_PROJ\)/src -I$$\(SIGNALSANDGATEWAYS_PROJ\)/src -L$$\(CORE4INET_PROJ\)/src -L$$\(FICO4OMNET_PROJ\)/src -L$$\(INET_PROJ\)/src -L$$\(SIGNALSANDGATEWAYS_PROJ\)/src -lCoRE4INET$$\(D\) -lFiCo4OMNeT$$\(D\) -lINET$$\(D\) -lSignalsAndGateways$$\(D\)
#

# Name of target to be created (-o option)
TARGET = $(LIB_PREFIX)CAN4CQF$(D)$(SHARED_LIB_SUFFIX)
TARGET_DIR = .

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -I$(CORE4INET_PROJ)/src \
    -I$(CORE4INET_PROJ)/src/core4inet/api/AS6802/tte_api \
    -I$(INET_PROJ)/src \
    -I$(FICO4OMNET_PROJ)/src \
    -I$(SIGNALSANDGATEWAYS_PROJ)/src

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(CORE4INET_PROJ)/src $(LDFLAG_LIBPATH)$(FICO4OMNET_PROJ)/src $(LDFLAG_LIBPATH)$(INET_PROJ)/src $(LDFLAG_LIBPATH)$(SIGNALSANDGATEWAYS_PROJ)/src  -lCoRE4INET$(D) -lFiCo4OMNeT$(D) -lINET$(D) -lSignalsAndGateways$(D)

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/can4cqf/applications/IEEE8021Qch/TTApplicationBase.o \
    $O/can4cqf/applications/ethernet/EthernetGatewayApplication.o \
    $O/can4cqf/gateway/buffering/AggregationGatewayBuffering.o \
    $O/can4cqf/gateway/buffering/CircularRotationBuffer.o \
    $O/can4cqf/gateway/buffering/NoGatewayBuffering.o \
    $O/can4cqf/gateway/buffering/ResortAggregationGatewayBuffering.o \
    $O/can4cqf/gateway/routing/BaseGatewayRouter.o \
    $O/can4cqf/gateway/transformation/EDFGatewayTransformation.o \
    $O/can4cqf/gateway/transformation/GatewayTransformation.o \
    $O/can4cqf/gateway/transformation/NoGatewayTransformation.o \
    $O/can4cqf/gateway/transformation/RAEGatewayTransformation.o \
    $O/can4cqf/gateway/transformation/TimeoutGatewayTransformation.o \
    $O/can4cqf/gateway/transformation/VarGatewayTransformation.o \
    $O/can4cqf/linklayer/inControl/InControls.o \
    $O/can4cqf/linklayer/message/ARTF.o \
    $O/can4cqf/linklayer/message/CircularRotationMessage.o \
    $O/can4cqf/linklayer/shaper/IEEE8021Qch/Rxgate.o \
    $O/can4cqf/linklayer/shaper/IEEE8021Qch/gate/IEEE8021QchGateControlList.o \
    $O/can4cqf/linklayer/shaper/IEEE8021Qch_queueing/IEEE8021QchQueueing.o \
    $O/can4cqf/linklayer/message/ARTF_m.o \
    $O/can4cqf/linklayer/message/CircularRotationMessage_m.o \
    $O/can4cqf/linklayer/message/HRTF_m.o \
    $O/can4cqf/linklayer/message/IEEE8021QchBEFlow_m.o \
    $O/can4cqf/linklayer/message/IEEE8021QchTTFlow_m.o \
    $O/can4cqf/linklayer/message/PoolMessage_m.o

# Message files
MSGFILES = \
    can4cqf/linklayer/message/ARTF.msg \
    can4cqf/linklayer/message/CircularRotationMessage.msg \
    can4cqf/linklayer/message/HRTF.msg \
    can4cqf/linklayer/message/IEEE8021QchBEFlow.msg \
    can4cqf/linklayer/message/IEEE8021QchTTFlow.msg \
    can4cqf/linklayer/message/PoolMessage.msg

# SM files
SMFILES =

# Other makefile variables (-K)
CORE4INET_PROJ=../../CoRE4INET
INET_PROJ=../../inet
FICO4OMNET_PROJ=../../FiCo4OMNeT
SIGNALSANDGATEWAYS_PROJ=../../SignalsAndGateways

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(TOOLCHAIN_NAME),clangc2)
LIBS += -Wl,-rpath,$(abspath $(CORE4INET_PROJ)/src) -Wl,-rpath,$(abspath $(FICO4OMNET_PROJ)/src) -Wl,-rpath,$(abspath $(INET_PROJ)/src) -Wl,-rpath,$(abspath $(SIGNALSANDGATEWAYS_PROJ)/src)
endif

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)