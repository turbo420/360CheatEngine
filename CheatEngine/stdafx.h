#pragma once

#include <xtl.h>
#include <xboxmath.h>
#include <stdio.h>
#include <string>
#include <ppcintrinsics.h>
#include <algorithm>
#include <vector>
#include <time.h>
#include <fstream>
#include <xkelib.h>

using namespace std;

#define XAPO_ALLOC_ATTRIBUTES MAKE_XALLOC_ATTRIBUTES (      \
            0,                           /* ObjectType */           \
            FALSE,                       /* HeapTracksAttributes */ \
            TRUE,                       /* MustSucceed */          \
            FALSE,                       /* FixedSize */            \
            eXALLOCAllocatorId_XBOXKERNEL,  /* AllocatorId */          \
            XALLOC_ALIGNMENT_DEFAULT,    /* Alignment */            \
            XALLOC_MEMPROTECT_WRITECOMBINE, /* MemoryProtect */        \
            TRUE,                       /* ZeroInitialize */       \
            XALLOC_MEMTYPE_HEAP          /* MemoryType */           \
)


#include "Log.h"
#include "Utils.h"
#include "SMC.h"
#include "structs.h"
#include "Global.h"
#include "Structure.h"

#include "Socket.h"
#include "WebSocket.h"
#include "Requests.h"

#include "Hypervisor.h"

#include "Hooking.h"

#include "SystemHooks.h"
#include "TitleHooks.h"