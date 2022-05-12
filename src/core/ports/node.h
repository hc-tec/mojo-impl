//
// Created by titto on 2022/5/8.
//

#ifndef MOJO_IMPL_NODE_H
#define MOJO_IMPL_NODE_H

#include <memory>
#include <unordered_map>

#include "base/mutex.h"
#include "base/rand_util.h"
#include "core/ports/name.h"
#include "core/ports/port.h"
#include "core/ports/port_ref.h"

namespace tit {
namespace mojo {
namespace ports {

template <typename T>
void GenerateRandomName(T* out) {
  base::RandBytes(out, sizeof(T));
}

enum : int8 {
  OK = 0,
  ERROR_PORT_UNKNOWN = -10,
  ERROR_PORT_EXISTS = -11,
  ERROR_PORT_STATE_UNEXPECTED = -12,
  ERROR_PORT_CANNOT_SEND_SELF = -13,
  ERROR_PORT_PEER_CLOSED = -14,
  ERROR_PORT_CANNOT_SEND_PEER = -15,
  ERROR_NOT_IMPLEMENTED = -100,
};

class Node {
 public:
  using Ptr = std::unique_ptr<Node>;

  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void ForwardEvent(const NodeName& node, const Event::Ptr & event) = 0;
    virtual void BroadcastEvent(const Event& event) = 0;
    virtual void PortStatusChanged(const PortRef& port_ref) = 0;
  };

  static Ptr Create(const NodeName& name, Delegate* delegate) {
    return std::make_unique<Node>(name, delegate);
  }

  Node(const NodeName& name, Delegate* delegate);
  ~Node();

  int CreateUninitializedPort(PortRef* port_ref);

  int InitializePort(const PortRef& port_ref,
                     const NodeName& peer_node_name,
                     const PortName& peer_port_name);

  int CreatePortPair(PortRef* port0_ref, PortRef* port1_ref);

  int AddPortWithName(const PortName& port_name, const Port::Ptr& port);

  void UpdatePortPeerAddress(const PortName& local_port_name,
                             const Port::Ptr& local_port,
                             const NodeName& new_peer_node,
                             const PortName& new_peer_port);

  int MergePorts(PortName* local_port_name,
                 const PortRef& port_ref,
                 const NodeName& destination_node_name,
                 const PortName& destination_port_name);

  int MergePortsResponse(const PortName& local_peer_port_name,
                 const NodeName& destination_node_name,
                 const PortName& destination_port_name);

  int GetMessage(const PortRef& port_ref,
                 Event::Ptr& event);

  int SendUserMessage(const PortRef& port_ref,
                      const Event::Ptr& message);

  int OnUserMessage(const NodeName& from_node,
                    const Event::Ptr& event);

  int ClosePort(const PortRef& port_ref);

  void ErasePort(const PortName& port_name);

  DISALLOW_COPY_AND_ASSIGN(Node);
 private:
  class DelegateHolder {
   public:
    DelegateHolder(Node* node, Delegate* delegate);

    DelegateHolder(const DelegateHolder&) = delete;
    DelegateHolder& operator=(const DelegateHolder&) = delete;

    ~DelegateHolder();

    Delegate* operator->() const {
      return delegate_;
    }

   private:
    Node* node_;
    Delegate* delegate_;
  };

  int SendUserMessageInternal(const PortRef& port_ref,
                              const Event::Ptr& message);

 private:
  NodeName name_;
  Delegate* delegate_;

  using LocalPortName = PortName;
  using PeerPortName = PortName;

  base::MutexLock ports_lock_;
  std::unordered_map<LocalPortName, Port::Ptr> ports_;

  using PeerPortMap = std::unordered_map<PeerPortName,
                         std::map<LocalPortName, PortRef>>;
  std::unordered_map<NodeName, PeerPortMap> peer_port_maps_;
};

}  // namespace ports
}  // namespace mojo
}  // namespace tit


#endif  // MOJO_IMPL_NODE_H
