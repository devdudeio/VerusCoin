#include <cJSON.h>
#include <stdint.h>


#ifndef CRYPTOCONDITIONS_H
#define CRYPTOCONDITIONS_H


#ifdef __cplusplus
extern "C" {
#endif


struct CC;
struct CCType;

#define MAX_BINARY_CC_SIZE 3072

enum CCTypeId {
    CC_Anon = -1,
    CC_Preimage = 0,
    CC_Prefix = 1,
    CC_Threshold = 2,
    CC_Ed25519 = 4,
    CC_Secp256k1 = 5,
    CC_Eval = 15,
    CC_Falcon512 = 31
};


/*
 * Evaliliary verification callback, fulfilled is zero if the node was not fulfilled in its signatures, 1 if it was
 */
typedef int (*VerifyEval)(struct CC *cond, void *context, int fulfilled);



/*
 * Crypto Condition
 */
typedef struct CC {
    struct CCType *type;
    union {
        // public key types
        struct { uint8_t *publicKey, *signature; };
        // preimage
        struct { uint8_t *preimage; size_t preimageLength; };
        // threshold
        struct { long threshold; uint8_t size; struct CC **subconditions; };
        // prefix
        struct { uint8_t *prefix; size_t prefixLength; struct CC *subcondition;
                 size_t maxMessageLength; };
        // eval
        struct { uint8_t *code; size_t codeLength; };
        // anon
        struct { uint8_t fingerprint[32]; uint32_t subtypes; unsigned long cost; 
                 struct CCType *conditionType; };
    };
} CC;

/*
 * Crypto Condition Visitor
 */
typedef struct CCVisitor {
    int (*visit)(CC *cond, struct CCVisitor visitor);
    const uint8_t *msg;
    size_t msgLength;
    void *context;
} CCVisitor;


/*
 * Public methods
 */
int             cc_isFulfilled(const CC *cond);
int             cc_verify(const struct CC *cond, const uint8_t *msg, size_t msgLength,
                        int doHashMessage, const uint8_t *condBin, size_t condBinLength,
                        VerifyEval verifyEval, void *evalContext, int checkSig);
int             cc_visit(CC *cond, struct CCVisitor visitor);
int             cc_isEvalVisitor(CCVisitor *visitor);
void            cc_setEvalVisitorFulfilled(CCVisitor *visitor, int fulfilled);
int             cc_isEvalVisitorFulfilled(CCVisitor *visitor);
int             cc_countEvals(const CC *cond);
int             cc_signTreeEd25519(CC *cond, const uint8_t *privateKey, const uint8_t *msg,
                        const size_t msgLength);
int             cc_signTreeSecp256k1Msg32(CC *cond, const uint8_t *privateKey, const uint8_t *msg32);
int             cc_secp256k1VerifyTreeMsg32(const CC *cond, const uint8_t *msg32);
size_t          cc_conditionBinary(const CC *cond, uint8_t *buf, int bufLen);
size_t          cc_fulfillmentBinary(const CC *cond, uint8_t *buf, size_t bufLength);
size_t          cc_partialFulfillmentBinary(const CC *cond, unsigned char *buf, size_t length);
struct CC*      cc_conditionFromJSON(cJSON *params, char *err);
struct CC*      cc_conditionFromJSONString(const char *json, char *err);
struct CC*      cc_readConditionBinary(const uint8_t *cond_bin, size_t cond_bin_len);
struct CC*      cc_readFulfillmentBinary(const uint8_t *ffill_bin, size_t ffill_bin_len);
int             cc_MakeSecp256k1Signature(const unsigned char *msg32, const unsigned char *privateKey, unsigned char **signatureOut);
int             cc_MakeFalcon512Signature(const unsigned char *msg32, const unsigned char *privateKey, unsigned char **signatureOut);
int             cc_MakeFalcon512KeyPair(unsigned char *privateKey, unsigned char *publicKey);
int             cc_VerifyFalcon512Key(const unsigned char *msg32, const unsigned char *publicKey, unsigned char *signature);
int             cc_ApplySecp256k1Signature(const CC *cond, const unsigned char *publicKey, const unsigned char *pubkeyHash20, const unsigned char *signature);
int             cc_readFulfillmentBinaryExt(const unsigned char *ffill_bin, size_t ffill_bin_len, CC **ppcc);
int             cc_readPartialFulfillmentBinaryExt(const unsigned char *ffill_bin, size_t ffill_bin_len, CC **ppcc);
struct CC*      cc_new(int typeId);
struct cJSON*   cc_conditionToJSON(const CC *cond);
char*           cc_conditionToJSONString(const CC *cond);
char*           cc_conditionUri(const CC *cond);
char*           cc_jsonRPC(char *request);
char*           cc_typeName(const CC *cond);
enum CCTypeId   cc_typeId(const CC *cond);
unsigned long   cc_getCost(const CC *cond);
uint32_t        cc_typeMask(const CC *cond);
int             cc_isAnon(const CC *cond);
void            cc_free(struct CC *cond);

#ifdef __cplusplus
}
#endif

#endif  /* CRYPTOCONDITIONS_H */
