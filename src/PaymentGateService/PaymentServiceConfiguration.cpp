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

#include "PaymentServiceConfiguration.h"

#include <iostream>
#include <algorithm>
#include <boost/program_options.hpp>

#include "Logging/ILogger.h"

namespace po = boost::program_options;

namespace PaymentService {

Configuration::Configuration() {
  generateNewContainer = false;
  daemonize = false;
  registerService = false;
  unregisterService = false;
  logFile = "payment_gate.log";
  testnet = false;
  printAddresses = false;
  logLevel = Logging::INFO;
  bindAddress = "";
  bindPort = 0;
  secretSpendKey = "";
  secretViewKey = "";
}

void Configuration::initOptions(boost::program_options::options_description& desc) {
  desc.add_options()
      ("bind-address", po::value<std::string>()->default_value("0.0.0.0"), "payment service bind address")
      ("bind-port", po::value<uint16_t>()->default_value(8070), "payment service bind port")
      ("rpc-user", po::value<std::string>()->default_value(""), "username to use the payment service. If authorization is not required, leave it empty")
      ("rpc-password", po::value<std::string>()->default_value(""), "password to use the payment service. If authorization is not required, leave it empty")
      ("container-file,w", po::value<std::string>(), "container file")
      ("container-password,p", po::value<std::string>(), "container password")
      ("generate-container,g", "generate new container file with one wallet and exit")
      ("view-key", po::value<std::string>(),"Generate a wallet container with this secret view <key>")
      ("spend-key", po::value<std::string>(),"Generate a wallet container with this secret spend <key>")
      ("daemon,d", "run as daemon in Unix or as service in Windows")
      ("log-file,l", po::value<std::string>(), "log file")
      ("server-root", po::value<std::string>(), "server root. The service will use it as working directory. Don't set it if don't want to change it")
      ("log-level", po::value<size_t>(), "log level")
      ("address", "print wallet addresses and exit");
}

void Configuration::init(const boost::program_options::variables_map& options) {
  if (options.count("daemon") != 0) {
    daemonize = true;
  }

  if (options.count("register-service") != 0) {
    registerService = true;
  }

  if (options.count("unregister-service") != 0) {
    unregisterService = true;
  }

  if (registerService && unregisterService) {
    throw ConfigurationError("It's impossible to use both \"register-service\" and \"unregister-service\" at the same time");
  }

  if (options["testnet"].as<bool>()) {
    testnet = true;
  }

  if (options.count("log-file") != 0) {
    logFile = options["log-file"].as<std::string>();
  }

  if (options.count("log-level") != 0) {
    logLevel = options["log-level"].as<size_t>();
    if (logLevel > Logging::TRACE) {
      std::string error = "log-level option must be in " + std::to_string(Logging::FATAL) +  ".." + std::to_string(Logging::TRACE) + " interval";
      throw ConfigurationError(error.c_str());
    }
  }

  if (options.count("server-root") != 0) {
    serverRoot = options["server-root"].as<std::string>();
  }

  if (options.count("bind-address") != 0 && (!options["bind-address"].defaulted() || bindAddress.empty())) {
    bindAddress = options["bind-address"].as<std::string>();
  }

  if (options.count("bind-port") != 0 && (!options["bind-port"].defaulted() || bindPort == 0)) {
    bindPort = options["bind-port"].as<uint16_t>();
  }

  if (options.count("rpc-user") != 0 && !options["rpc-user"].defaulted()) {
    rpcUser = options["rpc-user"].as<std::string>();
  }

  if (options.count("rpc-password") != 0 && !options["rpc-password"].defaulted()) {
    rpcPassword = options["rpc-password"].as<std::string>();
  }

  if (options.count("container-file") != 0) {
    containerFile = options["container-file"].as<std::string>();
  }

  if (options.count("view-key") != 0) {
    secretViewKey = options["view-key"].as<std::string>();
  }

  if (options.count("spend-key") != 0) {
    secretSpendKey = options["spend-key"].as<std::string>();
  }

  if (options.count("container-password") != 0) {
    containerPassword = options["container-password"].as<std::string>();
  }

  if (options.count("generate-container") != 0) {
    generateNewContainer = true;
  }

  if (options.count("address") != 0) {
    printAddresses = true;
  }

  if (!registerService && !unregisterService) {
    if (containerFile.empty() || containerPassword.empty()) {
      throw ConfigurationError("Both container-file and container-password parameters are required");
    }
  }
}

} //namespace PaymentService
