include Makefile.include

AM_CPPFLAGS += -I$(builddir)

noinst_LIBRARIES = \
  libbitcoin_server.a \
  libbitcoin_common.a \
  libbitcoin_cli.a
if ENABLE_WALLET
noinst_LIBRARIES += libbitcoin_wallet.a
endif

bin_PROGRAMS =

if BUILD_BITCOIND
  bin_PROGRAMS += unitusd
endif

if BUILD_BITCOIN_CLI
  bin_PROGRAMS += unitus-cli
endif

SUBDIRS = . $(BUILD_QT) $(BUILD_TEST)
DIST_SUBDIRS = . qt test
.PHONY: FORCE
# bitcoin core #
BITCOIN_CORE_H = \
  addrman.h \
  alert.h \
  auxpow.h \
  allocators.h \
  base58.h bignum.h \
  bloom.h \
  chainparams.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  core.h \
  crypter.h \
  db.h \
  hash.h \
  init.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
  miner.h \
  mruset.h \
  netbase.h \
  net.h \
  noui.h \
  protocol.h \
  rpcclient.h \
  rpcprotocol.h \
  rpcserver.h \
  script.h \
  serialize.h \
  sync.h \
  threadsafety.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  uint256.h \
  util.h \
  version.h \
  walletdb.h \
  wallet.h \
  crypto/sha3/sph_blake.h \
  crypto/sha3/sph_blakefast.h \
  crypto/sha3/sph_groestl.h \
  crypto/sha3/sph_keccak.h \
  crypto/sha3/sph_bmw.h \
  crypto/sha3/sph_jh.h \
  crypto/sha3/sph_skein.h \
  crypto/sha3/sph_types.h \
  crypto/sha3/sph_echo.h \
  crypto/sha3/sph_luffa.h \
  crypto/sha3/sph_simd.h \
  crypto/sha3/sph_cubehash.h \
  crypto/sha3/sph_shavite.h \
  crypto/hashskein.h \
  crypto/hashqubit.h \
  crypto/hashX11.h \
  crypto/hashblake.h \
  crypto/yescrypt/yescrypt.h \
  crypto/yescrypt/sysendian.h \
  crypto/yescrypt/sha256_Y.h \
  crypto/lyra2/lyra2.h \
  crypto/lyra2/lyra2RE.h \
  crypto/lyra2/sponge.h \
  crypto/hashargon2.h \
  crypto/argon2/argon2.h \
  crypto/argon2/blake2.h \
  crypto/argon2/blake2-impl.h \
  crypto/argon2/blamka-round-opt.h \
  crypto/argon2/core.h \
  crypto/argon2/encoding.h \
  crypto/argon2/opt.h \
  crypto/argon2/thread.h 

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(abs_top_builddir)/src/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
version.o: obj/build.h

libbitcoin_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  auxpow.cpp \
  bloom.cpp \
  checkpoints.cpp \
  coins.cpp \
  init.cpp \
  keystore.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  rpcblockchain.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  txdb.cpp \
  txmempool.cpp \
  $(JSON_H) \
  $(BITCOIN_CORE_H)

libbitcoin_wallet_a_SOURCES = \
  db.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  wallet.cpp \
  walletdb.cpp \
  $(BITCOIN_CORE_H)

libbitcoin_common_a_SOURCES = \
  base58.cpp \
  allocators.cpp \
  chainparams.cpp \
  core.cpp \
  hash.cpp \
  key.cpp \
  netbase.cpp \
  protocol.cpp \
  rpcprotocol.cpp \
  script.cpp \
  sync.cpp \
  util.cpp \
  version.cpp \
  crypto/sha3/blake.c \
  crypto/sha3/blakefast.c \
  crypto/sha3/bmw.c \
  crypto/sha3/groestl.c \
  crypto/sha3/jh.c \
  crypto/sha3/keccak.c \
  crypto/sha3/skein.c \
  crypto/sha3/aes_helper.c \
  crypto/sha3/echo.c \
  crypto/sha3/luffa.c \
  crypto/sha3/simd.c \
  crypto/sha3/cubehash.c \
  crypto/sha3/shavite.c \
  crypto/yescrypt/yescryptcommon.c \
  crypto/yescrypt/yescrypt-best.c \
  crypto/yescrypt/yescrypt-platform.c \
  crypto/yescrypt/sha256_Y.c \
  crypto/lyra2/lyra2.c \
  crypto/lyra2/lyra2RE.c \
  crypto/lyra2/sponge.c \
  crypto/argon2/argon2.c \
  crypto/argon2/blake2b.c \
  crypto/argon2/core.c \
  crypto/argon2/encoding.c \
  crypto/argon2/opt.c \
  crypto/argon2/thread.c \
  $(BITCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libbitcoin_common_a_SOURCES += compat/glibc_compat.cpp
libbitcoin_common_a_SOURCES += compat/glibcxx_compat.cpp
endif

libbitcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(BITCOIN_CORE_H)

nodist_libbitcoin_common_a_SOURCES = $(top_srcdir)/src/obj/build.h
#

# bitcoind binary #
unitusd_LDADD = \
  libbitcoin_server.a \
  libbitcoin_cli.a \
  libbitcoin_common.a \
  $(LIBLEVELDB) \
  $(LIBMEMENV)
if ENABLE_WALLET
unitusd_LDADD += libbitcoin_wallet.a
endif
unitusd_SOURCES = bitcoind.cpp
#

if TARGET_WINDOWS
unitusd_SOURCES += bitcoind-res.rc
endif

AM_CPPFLAGS += $(BDB_CPPFLAGS)
unitusd_LDADD += $(BOOST_LIBS) $(BDB_LIBS)

# bitcoin-cli binary #
unitus_cli_LDADD = \
  libbitcoin_cli.a \
  libbitcoin_common.a \
  $(BOOST_LIBS)
unitus_cli_SOURCES = bitcoin-cli.cpp
#

if TARGET_WINDOWS
unitus_cli_SOURCES += bitcoin-cli-res.rc
endif

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
leveldb/libleveldb.a: leveldb/libmemenv.a

leveldb/%.a:
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
	  OPT="$(CXXFLAGS) $(CPPFLAGS)"

qt/bitcoinstrings.cpp: $(libbitcoin_server_a_SOURCES) $(libbitcoin_common_a_SOURCES) $(libbitcoin_cli_a_SOURCES)
	@test -n $(XGETTEXT) || echo "xgettext is required for updating translations"
	@cd $(top_srcdir); XGETTEXT=$(XGETTEXT) share/qt/extract_strings_qt.py

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb Makefile.include

clean-local:
	-$(MAKE) -C leveldb clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
