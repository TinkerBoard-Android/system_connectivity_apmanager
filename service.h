// Copyright 2014 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef APMANAGER_SERVICE_H_
#define APMANAGER_SERVICE_H_

#include <string>

#include <base/macros.h>
#include <chromeos/process.h>

#include "apmanager/config.h"
#include "apmanager/dbus_adaptors/org.chromium.apmanager.Service.h"
#include "apmanager/dhcp_server_factory.h"
#include "apmanager/file_writer.h"
#include "apmanager/hostapd_monitor.h"
#include "apmanager/process_factory.h"

namespace apmanager {

class Manager;

class Service : public org::chromium::apmanager::ServiceAdaptor,
                public org::chromium::apmanager::ServiceInterface {
 public:
  Service(Manager* manager, int service_identifier);
  virtual ~Service();

  // Implementation of ServiceInterface.
  virtual bool Start(chromeos::ErrorPtr* error);
  virtual bool Stop(chromeos::ErrorPtr* error);

  // Register Service DBus object.
  void RegisterAsync(
      chromeos::dbus_utils::ExportedObjectManager* object_manager,
      const scoped_refptr<dbus::Bus>& bus,
      chromeos::dbus_utils::AsyncEventSequencer* sequencer);

  const dbus::ObjectPath& dbus_path() const { return dbus_path_; }

  int identifier() const { return identifier_; }

 private:
  friend class ServiceTest;

  static const char kHostapdPath[];
  static const char kHostapdConfigPathFormat[];
  static const char kHostapdControlInterfacePath[];
  static const int kTerminationTimeoutSeconds;
  static const char kStateIdle[];
  static const char kStateStarting[];
  static const char kStateStarted[];
  static const char kStateFailed[];

  // Return true if hostapd process is currently running.
  bool IsHostapdRunning();

  // Start hostapd process. Return true if process is created/started
  // successfully, false otherwise.
  bool StartHostapdProcess(const std::string& config_file_path);

  // Stop the running hostapd process. Sending it a SIGTERM signal first, then
  // a SIGKILL if failed to terminated with SIGTERM.
  void StopHostapdProcess();

  // Release resources allocated to this service.
  void ReleaseResources();

  void HostapdEventCallback(HostapdMonitor::Event event,
                            const std::string& data);

  Manager* manager_;
  int identifier_;
  std::string service_path_;
  dbus::ObjectPath dbus_path_;
  std::unique_ptr<Config> config_;
  std::unique_ptr<chromeos::dbus_utils::DBusObject> dbus_object_;
  std::unique_ptr<chromeos::Process> hostapd_process_;
  std::unique_ptr<DHCPServer> dhcp_server_;
  DHCPServerFactory* dhcp_server_factory_;
  FileWriter* file_writer_;
  ProcessFactory* process_factory_;
  std::unique_ptr<HostapdMonitor> hostapd_monitor_;

  DISALLOW_COPY_AND_ASSIGN(Service);
};

}  // namespace apmanager

#endif  // APMANAGER_SERVICE_H_