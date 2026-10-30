#include <unity.h>

#include "../../src/Services/CmsisDapProtocol.h"

void test_cmsis_dap_protocol_info_connect_disconnect() {
    const CmsisDapInfo info = {
        "Bit Pirate",
        "Bit Pirate CMSIS-DAP",
        "BP-DAP-01",
        0x01,
        64,
        1,
    };
    uint8_t response[64];

    const uint8_t vendorInfo[] = {CMSIS_DAP_INFO, 0x01};
    size_t length = cmsisDapHandleCommand(vendorInfo, sizeof(vendorInfo), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(13, length);
    TEST_ASSERT_EQUAL_UINT8(CMSIS_DAP_INFO, response[0]);
    TEST_ASSERT_EQUAL_UINT8(11, response[1]);
    TEST_ASSERT_EQUAL_STRING("Bit Pirate", reinterpret_cast<const char*>(response + 2));

    const uint8_t versionInfo[] = {CMSIS_DAP_INFO, 0x04};
    length = cmsisDapHandleCommand(versionInfo, sizeof(versionInfo), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(8, length);
    TEST_ASSERT_EQUAL_STRING("2.0.0", reinterpret_cast<const char*>(response + 2));

    const uint8_t capabilitiesInfo[] = {CMSIS_DAP_INFO, 0xF0};
    length = cmsisDapHandleCommand(capabilitiesInfo, sizeof(capabilitiesInfo), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(3, length);
    TEST_ASSERT_EQUAL_UINT8(0x01, response[2]);

    const uint8_t packetCount[] = {CMSIS_DAP_INFO, 0xFE};
    length = cmsisDapHandleCommand(packetCount, sizeof(packetCount), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(3, length);
    TEST_ASSERT_EQUAL_UINT8(1, response[2]);

    const uint8_t swjMode[] = {CMSIS_DAP_INFO, 0x05};
    length = cmsisDapHandleCommand(swjMode, sizeof(swjMode), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(3, length);
    TEST_ASSERT_EQUAL_UINT8(1, response[2]);

    const uint8_t packetSize[] = {CMSIS_DAP_INFO, 0xFF};
    length = cmsisDapHandleCommand(packetSize, sizeof(packetSize), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(4, length);
    TEST_ASSERT_EQUAL_UINT16(64, static_cast<uint16_t>(response[2] | (response[3] << 8)));

    const uint8_t connectSwd[] = {CMSIS_DAP_CONNECT, 0x01};
    length = cmsisDapHandleCommand(connectSwd, sizeof(connectSwd), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(2, length);
    TEST_ASSERT_EQUAL_UINT8(0x01, response[1]);

    const uint8_t connectJtag[] = {CMSIS_DAP_CONNECT, 0x02};
    length = cmsisDapHandleCommand(connectJtag, sizeof(connectJtag), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(2, length);
    TEST_ASSERT_EQUAL_UINT8(0x00, response[1]);

    const uint8_t disconnect[] = {CMSIS_DAP_DISCONNECT};
    length = cmsisDapHandleCommand(disconnect, sizeof(disconnect), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(2, length);
    TEST_ASSERT_EQUAL_UINT8(0x01, response[1]);

    const uint8_t unknown[] = {0x7E, 0x00};
    length = cmsisDapHandleCommand(unknown, sizeof(unknown), response, sizeof(response), info);
    TEST_ASSERT_EQUAL_UINT32(2, length);
    TEST_ASSERT_EQUAL_UINT8(0x7E, response[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, response[1]);

    const uint8_t truncatedInfo[] = {CMSIS_DAP_INFO};
    TEST_ASSERT_EQUAL_UINT32(
        0, cmsisDapHandleCommand(truncatedInfo, sizeof(truncatedInfo), response, sizeof(response), info));
}

void runCmsisDapProtocolTests() {
    RUN_TEST(test_cmsis_dap_protocol_info_connect_disconnect);
}
