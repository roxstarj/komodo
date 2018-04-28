/******************************************************************************
 * Copyright © 2014-2017 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

#include "komodo_defs.h"

uint32_t komodo_heightstamp(int32_t height);
void komodo_stateupdate(int32_t height,uint8_t notarypubs[][33],uint8_t numnotaries,uint8_t notaryid,uint256 txhash,uint64_t voutmask,uint8_t numvouts,uint32_t *pvals,uint8_t numpvals,int32_t kheight,uint32_t ktime,uint64_t opretvalue,uint8_t *opretbuf,uint16_t opretlen,uint16_t vout,uint256 MoM,int32_t MoMdepth);
void komodo_init(int32_t height);
int32_t komodo_MoMdata(int32_t *notarized_htp,uint256 *MoMp,uint256 *kmdtxidp,int32_t nHeight);
int32_t komodo_notarizeddata(int32_t nHeight,uint256 *notarized_hashp,uint256 *notarized_desttxidp);
char *komodo_issuemethod(char *userpass,char *method,char *params,uint16_t port);
void komodo_init(int32_t height);
void komodo_assetchain_pubkeys(char *jsonstr);
int32_t komodo_chosennotary(int32_t *notaryidp,int32_t height,uint8_t *pubkey33,uint32_t timestamp);
int32_t komodo_isrealtime(int32_t *kmdheightp);
uint64_t komodo_paxtotal();
int32_t komodo_longestchain();
uint64_t komodo_maxallowed(int32_t baseid);
int32_t komodo_bannedset(int32_t *indallvoutsp,uint256 *array,int32_t max);

pthread_mutex_t komodo_mutex;

#define KOMODO_ELECTION_GAP 2000    //((ASSETCHAINS_SYMBOL[0] == 0) ? 2000 : 100)
#define IGUANA_MAXSCRIPTSIZE 10001
#define KOMODO_ASSETCHAIN_MAXLEN 65

struct pax_transaction *PAX;
int32_t NUM_PRICES; uint32_t *PVALS;
struct knotaries_entry *Pubkeys;

struct komodo_state KOMODO_STATES[34];

#define _COINBASE_MATURITY 100
int COINBASE_MATURITY = _COINBASE_MATURITY;//100;

int32_t IS_KOMODO_NOTARY,USE_EXTERNAL_PUBKEY,KOMODO_CHOSEN_ONE,ASSETCHAINS_SEED,KOMODO_ON_DEMAND,KOMODO_EXTERNAL_NOTARIES,KOMODO_PASSPORT_INITDONE,KOMODO_PAX,KOMODO_EXCHANGEWALLET,KOMODO_REWIND;
int32_t KOMODO_LASTMINED,prevKOMODO_LASTMINED,JUMBLR_PAUSE,ASSETCHAINS_CC;
std::string NOTARY_PUBKEY,ASSETCHAINS_NOTARIES,ASSETCHAINS_OVERRIDE_PUBKEY;
uint8_t NOTARY_PUBKEY33[33],ASSETCHAINS_OVERRIDE_PUBKEY33[33];

char ASSETCHAINS_SYMBOL[KOMODO_ASSETCHAIN_MAXLEN];
uint16_t ASSETCHAINS_PORT;
uint32_t ASSETCHAIN_INIT;
uint32_t ASSETCHAINS_MAGIC = 2387029918;
uint64_t ASSETCHAINS_SUPPLY = 10;
uint64_t ASSETCHAINS_COMMISSION;
int64_t ASSETCHAINS_GENESISTXVAL = 5000000000;

// consensus variables for coinbase timelock control and timelock transaction support
// time locks are specified enough to enable their use initially to lock specific coinbase transactions for emission control
// to be verifiable, timelocks require additional data that enables them to be validated and their ownership and
// release time determined from the blockchain. to do this, every time locked output according to this
// spec will use an op_return with CLTV at front and anything after |OP_RETURN|PUSH of rest|OPRETTYPE_TIMELOCK|script|
#define _ASSETCHAINS_TIMELOCKOFF -1
int64_t ASSETCHAINS_TIMELOCKGTE = _ASSETCHAINS_TIMELOCKOFF;
uint64_t ASSETCHAINS_TIMEUNLOCKFROM = 0, ASSETCHAINS_TIMEUNLOCKTO = 0;

uint32_t ASSETCHAINS_LASTERA = 1;
uint64_t ASSETCHAINS_ENDSUBSIDY[ASSETCHAINS_MAX_ERAS],ASSETCHAINS_REWARD[ASSETCHAINS_MAX_ERAS],ASSETCHAINS_HALVING[ASSETCHAINS_MAX_ERAS],ASSETCHAINS_DECAY[ASSETCHAINS_MAX_ERAS];

#define _ASSETCHAINS_EQUIHASH 0
uint32_t ASSETCHAINS_NUMALGOS = 2;
uint32_t ASSETCHAINS_EQUIHASH = _ASSETCHAINS_EQUIHASH;
uint32_t ASSETCHAINS_VERUSHASH = 1;
const char *ASSETCHAINS_ALGORITHMS[] = {"equihash", "verushash"};
uint64_t ASSETCHAINS_NONCEMASK[] = {0xffff,0xffffff};
uint32_t ASSETCHAINS_NONCESHIFT[] = {32,40};
uint32_t ASSETCHAINS_HASHESPERROUND[] = {1,128};
uint32_t ASSETCHAINS_ALGO = _ASSETCHAINS_EQUIHASH;

uint32_t KOMODO_INITDONE;
char KMDUSERPASS[4096],BTCUSERPASS[4096]; uint16_t KMD_PORT = 7771,BITCOIND_PORT = 7771;
uint64_t PENDING_KOMODO_TX;

struct komodo_kv *KOMODO_KV;
pthread_mutex_t KOMODO_KV_mutex;
