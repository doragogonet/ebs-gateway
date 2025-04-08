#include "d_encoding.h"
#include <math.h>
#include "async_func.h"

// 获取对应类型的分区表
const PartitionRule* get_partition_table(EpcHeader header) {
    switch(header) {
    case HEADER_SGTIN96: return PARTITION_TABLES[0];  // SGTIN-96分区表
    case HEADER_SSCC96:  return PARTITION_TABLES[1];  // SSCC-96分区表
    case HEADER_SGLN96:  return PARTITION_TABLES[2];  // SGLN-96分区表
    case HEADER_GRAI96:  return PARTITION_TABLES[3];  // GRAI-96分区表
    case HEADER_GIAI96:  return PARTITION_TABLES[4];  // GIAI-96分区表
    case HEADER_GDTI96:  return PARTITION_TABLES[5];  // GDTI-96分区表
    case HEADER_GSRN96:  return PARTITION_TABLES[6];  // GSRN-96分区表
    case HEADER_GSCN96:  return PARTITION_TABLES[7];  // GSCN-96分区表
    case HEADER_CPID96:  return PARTITION_TABLES[8];  // CPID-96分区表
    default: return NULL;
    }
}

// 二进制转换工具
static void uint_to_bin(uint64_t value, int bits, char* output) {   
    for (int i = bits - 1; i >= 0; --i) {
        output[bits - 1 - i] = (value & (1ULL << i)) ? '1' : '0';
    }
    output[bits] = '\0'; // 終端文字を追加
}

// 编码主函数
int encode_epc(const EpcData* data, char* hex_buffer) {
    char bin_str[EPC_BIN_LENGTH+1] = {0};
    const PartitionRule* pr_table = get_partition_table(EpcHeader(data->header));
    
    // 步骤1：验证分区有效性
    int valid_partition = 0;
    
//    for(int i=0; pr_table[i].partition != 0; i++) {
    for(int i=0; i < sizeof(PARTITION_TABLES[0]) / sizeof(PARTITION_TABLES[0][0]); i++) {
        if(pr_table[i].partition == data->partition) {
            valid_partition = 1;
            break;
        }
    }
    if(!valid_partition) return -1;

    // 步骤2：构建二进制字符串
    int pos = 0;
    
    // Header (8 bits)
    uint_to_bin(data->header, 8, &bin_str[pos]);
    pos += 8;
    
    // Filter (3 bits)
    uint_to_bin(data->filter, 3, &bin_str[pos]);
    pos += 3;
    
    // Partition (3 bits)
    uint_to_bin(data->partition, 3, &bin_str[pos]);
    pos += 3;
    
    // 公司前缀和项目引用
    const PartitionRule* pr = &pr_table[data->partition];
    uint_to_bin(data->company_prefix, pr->cp_bits, &bin_str[pos]);
    pos += pr->cp_bits;
    uint_to_bin(data->item_ref, pr->ir_bits, &bin_str[pos]);
    pos += pr->ir_bits;
    
    // 序列号处理（不同类型不同长度）
	switch(data->header) {
		case HEADER_SGTIN96:
		    uint_to_bin(data->serial, 38, &bin_str[pos]);
		    pos += 38;
		    break;
		case HEADER_SGLN96:
		    // 位置编码：序列号固定38位（扩展位+位置代码）
		    uint_to_bin(data->extension, 1, &bin_str[pos]);
		    pos += 1;
		    uint_to_bin(data->serial, 37, &bin_str[pos]);
		    pos += 37;
		    break;
		case HEADER_GSRN96:
		    // 服务参考号：无序列号，直接填充保留位
		    memset(&bin_str[pos], '0', 24); // 填充24位保留位
		    pos += 24;
		    break;
		case HEADER_GSCN96:
		   // 集装箱编码：序列号分两部分处理
		    uint_to_bin(data->serial >> 24, 24, &bin_str[pos]);
		    pos += 24;
		    uint_to_bin(data->serial & 0xFFFFFF, 24, &bin_str[pos]);
		    pos += 24;
		    break;
		case HEADER_GIAI96:
		    // 资产编码：序列号为全资产编号（52bit 0r 58bit）
            if (data->partition == 5 && 58 - pr->ir_bits > 0 ) {
                uint_to_bin(data->serial, 58 - pr->ir_bits, &bin_str[pos]);
                pos += 58 - pr->ir_bits;
            }

            if (data->partition == 3 && 52 - pr->ir_bits > 0 ){
                uint_to_bin(data->serial, 52 - pr->ir_bits, &bin_str[pos]);
                pos += 52 - pr->ir_bits;
            }
		    break;
		case HEADER_GDTI96:
		    // 文档编码：序列号41bit（文档类型+序号）
		    uint_to_bin(data->item_ref, 41, &bin_str[pos]);
		    pos += 41;
		    break;
		case HEADER_CPID96:
			// 组件编码：序列号分三段（CAGE码+批次+序号）
		    uint_to_bin(data->serial >> 32, 16, &bin_str[pos]);
		    pos += 16;
		    uint_to_bin((data->serial >> 16) & 0xFFFF, 16, &bin_str[pos]);
		    pos += 16;
		    uint_to_bin(data->serial & 0xFFFF, 16, &bin_str[pos]);
		    pos += 16;
		    break;
		case HEADER_GRAI96:
		    uint_to_bin(data->serial, 38, &bin_str[pos]);
		    pos += 38;
		    break;
		case HEADER_SSCC96:
		    uint_to_bin(data->extension, 24, &bin_str[pos]);
		    pos += 24;
		    break;
		default:
		    return -1; // 未知类型错误
	}

    // 最終位置が96ビットを超えていないかチェック
    if (pos > EPC_BIN_LENGTH) {
        fprintf(stderr, "Error: Binary string overflow (%d bits)\n", pos);
        return -1;
    }


    // 步骤3：转换为十六进制
    for(int i=0; i<EPC_HEX_LENGTH; i++) {
        uint8_t nibble = 0;
        for(int j=0; j<4; j++) {
            nibble |= (bin_str[i*4+j] == '1') << (3-j);
        }
        hex_buffer[i] = "0123456789ABCDEF"[nibble];
    }
    hex_buffer[EPC_HEX_LENGTH] = '\0';
    
    return 0;
}

// 解码主函数
EpcData decode_epc(const char* hex_str) {

    EpcData epc_data = {0};
    char bin_str[EPC_BIN_LENGTH+1] = {0};
    
    // 十六进制转二进制
    for(int i=0; i<EPC_HEX_LENGTH; i++) {
        uint8_t nibble = hex_str[i] <= '9' ? 
            hex_str[i] - '0' : hex_str[i] - 'A' + 10;
        for(int j=0; j<4; j++) {
            bin_str[i*4+j] = (nibble & (8 >> j)) ? '1' : '0';
        }
    }
    
    // 解析Header
    for(int i=0; i<8; i++) {
        epc_data.header |= (bin_str[i] == '1') << (7-i);
    }
    
    // 获取分区表
    const PartitionRule* pr_table = get_partition_table(EpcHeader(epc_data.header));
    
    if (!pr_table) return { 0 };

    // 解析Filter和Partition
    for(int i=8; i<11; i++) 
        epc_data.filter |= (bin_str[i] == '1') << (10-i);
    for(int i=11; i<14; i++)
        epc_data.partition |= (bin_str[i] == '1') << (13-i);
    
    // 定位字段位置
    const PartitionRule* pr = &pr_table[epc_data.partition];
    if (!pr) return { 0 };

    int cp_start = 14;
    int ir_start = cp_start + pr->cp_bits;
    
    // 解码公司前缀和项目引用
    for(int i=0; i<pr->cp_bits; i++)
        epc_data.company_prefix |= (uint64_t)(bin_str[cp_start+i] == '1') << (pr->cp_bits-1 - i);
    for(int i=0; i<pr->ir_bits; i++)
        epc_data.item_ref |= (uint64_t)(bin_str[ir_start+i] == '1') << (pr->ir_bits-1 - i);
    
    // 解码序列号
    int serial_start = ir_start + pr->ir_bits;
    int serial_bits = EPC_BIN_LENGTH - serial_start;
    for(int i=0; i<serial_bits; i++)
        epc_data.serial |= (uint64_t)(bin_str[serial_start+i] == '1') << (serial_bits-1 - i);
    
    return epc_data;
}

// JAN生成函数
int generate_jan(EpcData* epcdata, char* jan_code) {
    if (epcdata->header != HEADER_SGTIN96) {
        return -1;
    }

    // 获取分区规则
    const PartitionRule* table = get_partition_table(EpcHeader(epcdata->header) );
    if (!table) return -1;

    // 查找匹配的分区规则
    const PartitionRule* rule = NULL;
    for (int i = 0; table[i].partition != 0 || i < 8; i++) {
        if (table[i].partition == epcdata->partition) {
            rule = &table[i];
            break;
        }
    }

    // 组合公司前缀和商品编号
    uint64_t combined = epcdata->company_prefix * (uint64_t)pow(10, rule->ir_digits) + epcdata->item_ref;

    // 格式化为13位数字
    snprintf(jan_code, 14, "%013llu", combined);


    // 校验位計算（EAN‐13）
    int sum = 0;
    for (int i = 0; i < 12; i++) {
        int digit = jan_code[i] - '0';
        // インデックスが偶数 (1桁目, 3桁目, ...) の場合は×1、奇数 (2桁目, 4桁目, ...) の場合は×3
        sum += (i % 2 == 0) ? digit : digit * 3;
    }
    jan_code[12] = (10 - (sum % 10)) % 10 + '0';  // チェックディジットの計算


    return 0;
}


void ClientData::handle_decode(cJSON* request) {
 
    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        send_error_response("Missing 'params' field",id->valuestring);
        return;
    }

    cJSON* epcjson = cJSON_GetObjectItem(params, "epc");
    if (!epcjson || !cJSON_IsString(epcjson)) {
        send_error_response("Missing 'params' epc",id->valuestring);
        return;
    }
    char epc[EPC_HEX_LENGTH + 1] = {0};

    memcpy(epc, epcjson->valuestring, EPC_HEX_LENGTH);
    EpcData decoded = decode_epc(epc);
 
    // 创建 JSON 对象
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "id", id->valuestring);

    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(response_json, "header", decoded.header);
    cJSON_AddNumberToObject(response_json, "filter", decoded.filter);
    cJSON_AddNumberToObject(response_json, "partition", decoded.partition);
    cJSON_AddNumberToObject(response_json, "company_prefix", decoded.company_prefix);
    cJSON_AddNumberToObject(response_json, "item_ref", decoded.item_ref);
    cJSON_AddNumberToObject(response_json, "serial", decoded.serial);
    
    char jan[15] = { 0 };
    if( 0 == generate_jan(&decoded, jan))
        cJSON_AddStringToObject(response_json, "JAN", jan);

    cJSON_AddItemToObject(root, "data", response_json);
    send_json_response(root);
}


void ClientData::handle_encode(cJSON* request) {

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        send_error_response("Missing 'params' field", id->valuestring);
        return;
    }


    cJSON* header = cJSON_GetObjectItem(params, "header");
    if (!header) {
        send_error_response("Missing 'params' header", id->valuestring);
        return;
    }

    cJSON* filter = cJSON_GetObjectItem(params, "filter");
    if (!filter) {
        send_error_response("Missing 'params' filter",id->valuestring);
        return;
    }

    cJSON* partition = cJSON_GetObjectItem(params, "partition");
    if (!partition) {
        send_error_response("Missing 'params' partition", id->valuestring);
        return;
    }
    cJSON* company_prefix = cJSON_GetObjectItem(params, "company_prefix");
    if (!company_prefix) {
        send_error_response("Missing 'params' company_prefix", id->valuestring);
        return;
    }
    cJSON* item_ref = cJSON_GetObjectItem(params, "item_ref");
    if (!item_ref) {
        send_error_response("Missing 'params' item_ref", id->valuestring);
        return;
    }

    cJSON* serial = cJSON_GetObjectItem(params, "serial");
    if (!serial) {
        send_error_response("Missing 'params' serial", id->valuestring);
        return;
    }


    EpcData encoded = { 0 };
    encoded.company_prefix = company_prefix->valuedouble;
    encoded.header = header->valueint;
    encoded.filter = filter->valueint;
    encoded.item_ref = item_ref->valuedouble;
    encoded.partition = partition->valueint;
    encoded.serial = serial->valuedouble;

    char epc[EPC_HEX_LENGTH + 1] = { 0 };

    if (encode_epc(&encoded, epc) == 0) {
      
        // 创建 JSON 对象
        cJSON* root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "status", "success");
        cJSON_AddStringToObject(root, "id", id->valuestring);

        cJSON* response_json = cJSON_CreateObject();
        cJSON_AddStringToObject(response_json, "epc", epc);
        cJSON_AddItemToObject(root, "data", response_json);
        send_json_response(root);

    }else{
       send_error_response("encode error!", id->valuestring);
    }
    return;
}
