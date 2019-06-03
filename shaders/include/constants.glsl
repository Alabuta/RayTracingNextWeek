#ifndef CONSTANTS_H
#define CONSTANTS_H


#define USE_SCHEDULER 0

const uint VERTEX_SEMANTIC_POSITION = 0u;

const uint SAMPLING_NUMBER = 64u;
const uint BOUNCES_NUMBER = 64u;

const uint kATOMIC_COUNTER_BINDING = 0u;
const uint kLAMBERTIAN_BUFFER_BINDING = 2u;
const uint kMETAL_BUFFER_BINDING = 3u;
const uint kDIELECTRIC_BUFFER_BINDING = 4u;
const uint kUNIT_VECTORS_BUFFER_BINDING = 5u;
const uint kPRIMITIVES_BINDING = 6u;
const uint kCAMERA_BINDING = 7u;

const uint kOUT_IMAGE_BINDING = 2u;

const uint kFRAME_NUMBER_UNIFORM_LOCATION = 1u;

const uint kSPHERES_NUMBER = 6u;
//const uint kUNIT_VECTORS_NUMBER = 8192u;

const uvec3 kGROUP_SIZE = uvec3(8, 8, 1);
const uint kGROUP_FLAT_SIZE = kGROUP_SIZE.x * kGROUP_SIZE.y * kGROUP_SIZE.z;

const uint kLOCAL_DATA_LENGTH = kGROUP_FLAT_SIZE * SAMPLING_NUMBER;

#endif    // CONSTANTS_H
