/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2021 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#ifndef NODEGRAPH_H
#define NODEGRAPH_H

#include "node/node.h"

namespace olive {

/**
 * @brief A collection of nodes
 *
 * This doesn't technically need to be a derivative of Item, but since both Item and NodeGraph need
 * to be QObject derivatives, this simplifies Sequence.
 */
class NodeGraph : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief NodeGraph Constructor
   */
  NodeGraph();

  /**
   * @brief NodeGraph Destructor
   */
  virtual ~NodeGraph() override;

  /**
   * @brief Destructively destroys all nodes in the graph
   */
  void Clear();

  /**
   * @brief Retrieve a complete list of the nodes belonging to this graph
   */
  const QVector<Node*>& nodes() const
  {
    return node_children_;
  }

  const QVector<Node*>& default_nodes() const
  {
    return default_nodes_;
  }

  bool NodeMapContainsNode(Node* node, Node* context) const
  {
    return position_map_.value(context).contains(node);
  }

  QPointF GetNodePosition(Node* node, Node* context)
  {
    return position_map_.value(context).value(node);
  }

  void SetNodePosition(Node* node, Node* context, const QPointF& pos)
  {
    position_map_[context].insert(node, pos);
    emit NodePositionAdded(node, context, pos);
  }

  void RemoveNodePosition(Node* node, Node* context)
  {
    PositionMap& map = position_map_[context];
    map.remove(node);
    if (map.isEmpty()) {
      position_map_.remove(context);
    }
    emit NodePositionRemoved(node, context);
  }

  bool ContextContainsNode(Node *node, Node *context)
  {
    return position_map_[context].contains(node);
  }

  qreal GetNodeContextHeight(Node *context);

  using PositionMap = QHash<Node*, QPointF>;

  const PositionMap &GetNodesForContext(Node *context)
  {
    return position_map_[context];
  }

  const QMap<Node *, PositionMap> &GetPositionMap() const
  {
    return position_map_;
  }

  int GetNumberOfContextsNodeIsIn(Node *node) const;

  bool NodeOutputsToContext(Node *node) const;

signals:
  /**
   * @brief Signal emitted when a Node is added to the graph
   */
  void NodeAdded(Node* node);

  /**
   * @brief Signal emitted when a Node is removed from the graph
   */
  void NodeRemoved(Node* node);

  void InputConnected(const NodeOutput& output, const NodeInput& input);

  void InputDisconnected(const NodeOutput& output, const NodeInput& input);

  void ValueChanged(const NodeInput& input);

  void NodePositionAdded(Node *node, Node *relative, const QPointF &position);

  void NodePositionRemoved(Node *node, Node *relative);

protected:
  void AddDefaultNode(Node* n)
  {
    default_nodes_.append(n);
  }

  virtual void childEvent(QChildEvent* event) override;

private:
  QVector<Node*> node_children_;

  QVector<Node*> default_nodes_;

  QMap<Node *, PositionMap> position_map_;

  PositionMap root_position_map_;

};

}

#endif // NODEGRAPH_H
