// Copyright (c) 2019-2021 Fango Developers
// Copyright (c) 2018-2021 Fandom Gold Society
// Copyright (c) 2018-2019 Conceal Network & Conceal Devs
// Copyright (c) 2016-2019 The Karbowanec developers
// Copyright (c) 2012-2018 The CryptoNote developers
//
// This file is part of Fango.
//
// Fango is free software distributed in the hope that it
// will be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. You can redistribute it and/or modify it under the terms
// of the GNU General Public License v3 or later versions as published
// by the Free Software Foundation. Fango includes elements written 
// by third parties. See file labeled LICENSE for more details.
// You should have received a copy of the GNU General Public License
// along with Fango. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "IWallet.h"
#include "WalletIndices.h"
#include "Common/IInputStream.h"
#include "Common/IOutputStream.h"
#include "Transfers/TransfersSynchronizer.h"
#include "Serialization/BinaryInputStreamSerializer.h"

#include "crypto/chacha8.h"

namespace CryptoNote
{

struct CryptoContext
{
  Crypto::chacha8_key key;
  Crypto::chacha8_iv iv;

  void incIv();
};

class WalletSerializer
{
public:
  WalletSerializer(
      ITransfersObserver &transfersObserver,
      Crypto::PublicKey &viewPublicKey,
      Crypto::SecretKey &viewSecretKey,
      uint64_t &actualBalance,
      uint64_t &pendingBalance,
      WalletsContainer &walletsContainer,
      TransfersSyncronizer &synchronizer,
      UnlockTransactionJobs &unlockTransactions,
      WalletTransactions &transactions,
      WalletTransfers &transfers,
      uint32_t transactionSoftLockTime,
      UncommitedTransactions &uncommitedTransactions);

  void save(const std::string &password, Common::IOutputStream &destination, bool saveDetails, bool saveCache);
  void load(const Crypto::chacha8_key &key, Common::IInputStream &source);

private:
  static const uint32_t SERIALIZATION_VERSION;

  void loadWallet(Common::IInputStream &source, const Crypto::chacha8_key &key, uint32_t version);
  void loadWalletV1(Common::IInputStream &source, const Crypto::chacha8_key &key);

  CryptoContext generateCryptoContext(const std::string &password);

  void saveVersion(Common::IOutputStream &destination);
  void saveIv(Common::IOutputStream &destination, Crypto::chacha8_iv &iv);
  void saveKeys(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void savePublicKey(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveSecretKey(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveFlags(bool saveDetails, bool saveCache, Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveWallets(Common::IOutputStream &destination, bool saveCache, CryptoContext &cryptoContext);
  void saveBalances(Common::IOutputStream &destination, bool saveCache, CryptoContext &cryptoContext);
  void saveTransfersSynchronizer(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveUnlockTransactionsJobs(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveUncommitedTransactions(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveTransactions(Common::IOutputStream &destination, CryptoContext &cryptoContext);
  void saveTransfers(Common::IOutputStream &destination, CryptoContext &cryptoContext);

  uint32_t loadVersion(Common::IInputStream &source);
  void loadIv(Common::IInputStream &source, Crypto::chacha8_iv &iv);
  void generateKey(const std::string &password, Crypto::chacha8_key &key);
  void loadKeys(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadPublicKey(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadSecretKey(Common::IInputStream &source, CryptoContext &cryptoContext);
  void checkKeys();
  void loadFlags(bool &details, bool &cache, Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadWallets(Common::IInputStream &source, CryptoContext &cryptoContext);
  void subscribeWallets();
  void loadBalances(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadTransfersSynchronizer(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadObsoleteSpentOutputs(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadUnlockTransactionsJobs(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadObsoleteChange(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadUncommitedTransactions(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadTransactions(Common::IInputStream &source, CryptoContext &cryptoContext);
  void loadTransfers(Common::IInputStream &source, CryptoContext &cryptoContext, uint32_t version);

  void loadWalletV1Keys(CryptoNote::BinaryInputStreamSerializer &serializer);
  void loadWalletV1Details(CryptoNote::BinaryInputStreamSerializer &serializer);
  void addWalletV1Details(const std::vector<WalletLegacyTransaction> &txs, const std::vector<WalletLegacyTransfer> &trs);
  void initTransactionPool();
  void resetCachedBalance();
  void updateTransactionsBaseStatus();
  void updateTransfersSign();

  ITransfersObserver &m_transfersObserver;
  Crypto::PublicKey &m_viewPublicKey;
  Crypto::SecretKey &m_viewSecretKey;
  uint64_t &m_actualBalance;
  uint64_t &m_pendingBalance;
  WalletsContainer &m_walletsContainer;
  TransfersSyncronizer &m_synchronizer;
  UnlockTransactionJobs &m_unlockTransactions;
  WalletTransactions &m_transactions;
  WalletTransfers &m_transfers;
  uint32_t m_transactionSoftLockTime;
  UncommitedTransactions &uncommitedTransactions;
};

} //namespace CryptoNote
