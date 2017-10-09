#ifndef PhiOS_DRIVERS_ACPI_SRAT
#define PhiOS_DRIVERS_ACPI_SRAT

#include "include/types.h"
#include "include/errors.h"
#include "include/compiler.h"

#include "drivers/acpi/include/SDTHeader.h"

/*
 *  SRAT header
 */

#define SRAT_PROCESSOR_LOCAL_APIC_TYPE      0
#define SRAT_MEMORY_TYPE                    1
#define SRAT_PROCESSOR_LOCAL_x2APIC_TYPE    2

struct _SRATHeader
{
    SDTHeader sdt;
    uint8     reserved[12];
} PhiOS_PACKED_STRUCTURE;

typedef struct _SRATHeader   SRATHeader;
typedef struct _SRATHeader* PSRATHeader;

struct _SRAT
{
    SRATHeader header;
    uint8 *ptr;
};

typedef struct _SRAT   SRAT;
typedef struct _SRAT* PSRAT;

/*
 *  SRAT processor local apic structure
 */

#define PROCESSOR_LOCAL_APIC_FLAG_ENABLED 1

struct _SRATProcessorLocalApic
{
    uint8   type;               // 0x0 for this type of structure.
    uint8   length;             // 16
    uint8   lowDomain;          // Bits [0:7] of the proximity domain.
    uint8   apicId;             // The processor's local APIC ID.
    uint32  flags;              // Flags.
    uint8   sapicEId;           // The processor's local SAPIC EID.
    uint8   highDomain[3];      // Bits [8:31] of the proximity domain.
    uint32  clockDomain;        // The clock domain to which the processor belongs.
} PhiOS_PACKED_STRUCTURE;

typedef struct _SRATProcessorLocalApic   SRATProcessorLocalApic;
typedef struct _SRATProcessorLocalApic* PSRATProcessorLocalApic;

/*
 *  SRAT memory structure
 */

#define MEMORY_FLAG_ENABLED         1
#define MEMORY_FLAG_HOT_PLUGGABLE   2
#define MEMORY_FLAG_NON_VOLATILE    4

struct _SRATMemory
{
    uint8   type;           // 0x1 for this type of structure.
    uint8   length;         // 40
    uint32  domain;         // Integer that represents the proximity domain
                            // to which the processor belongs.
    uint8   reserved1[2];   // Reserved.
    uint32  lowBase;        // Low 32 bits of the base address of the memory range.
    uint32  highBase;       // High 32 bits of the base address of the memory range.
    uint32  lowLength;      // Low 32 bits of the length of the range.
    uint32  highLength;     // High 32 bits of the length of the range.
    uint8   reserved2[4];   // Reserved.
    uint32  flags;          // Flags.
    uint8   reserved3[8];   // Reserved.
} PhiOS_PACKED_STRUCTURE;

typedef struct _SRATMemory   SRATMemory;
typedef struct _SRATMemory* PSRATMemory;

/*
 *  SRAT processor local x2 apic structure
 */

#define PROCESSOR_LOCAL_x2APIC_FLAG_ENABLED 1

struct _SRATProcessorLocalx2Apic
{
    uint8   type;           // 0x2 for this type of structure.
    uint8   length;         // 24
    uint8   reserved1[2];   // Reserved.
    uint32  domain;         // The proximity domain to which the logical processor belongs.
    uint32  x2apicId;       // The processor local x2APIC ID.
    uint32  flags;          // Flags.
    uint32  clockDomain;    // The clock domain to which the logical processor belongs.
    uint8   reserved2[4];   // Reserved.
} PhiOS_PACKED_STRUCTURE;

typedef struct _SRATProcessorLocalx2Apic   SRATProcessorLocalx2Apic;
typedef struct _SRATProcessorLocalx2Apic* PSRATProcessorLocalx2Apic;

#endif
