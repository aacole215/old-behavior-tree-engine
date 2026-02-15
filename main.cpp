#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

enum class Status {
    Success,
    Failure,
    Running
};

// shared state between nodes
class Blackboard {
public:
    void setInt(const std::string& key, int value) {
        data[key] = value;
    }

    int getInt(const std::string& key) const {
        auto it = data.find(key);
        return (it != data.end()) ? it->second : 0;
    }

private:
    std::unordered_map<std::string, int> data;
};

// base node
class Node {
public:
    virtual Status tick(Blackboard& bb) = 0;
    virtual void reset() {}
    virtual ~Node() = default;
};

// composite Base
class Composite : public Node {
protected:
    std::vector<std::shared_ptr<Node>> children;
    size_t activeChild = 0;

public:
    void addChild(const std::shared_ptr<Node>& child) {
        children.push_back(child);
    }

    void reset() override {
        activeChild = 0;
        for (auto& child : children)
            child->reset();
    }
};

// sequence runs until failure
class Sequence : public Composite {
public:
    Status tick(Blackboard& bb) override {
        while (activeChild < children.size()) {
            Status s = children[activeChild]->tick(bb);

            if (s == Status::Running)
                return Status::Running;

            if (s == Status::Failure) {
                reset();
                return Status::Failure;
            }

            activeChild++;
        }

        reset();
        return Status::Success;
    }
};

// selector runs until Success
class Selector : public Composite {
public:
    Status tick(Blackboard& bb) override {
        while (activeChild < children.size()) {
            Status s = children[activeChild]->tick(bb);

            if (s == Status::Running)
                return Status::Running;

            if (s == Status::Success) {
                reset();
                return Status::Success;
            }

            activeChild++;
        }

        reset();
        return Status::Failure;
    }
};

// condition wrapper node
class ConditionNode : public Node {
public:
    explicit ConditionNode(std::function<bool(Blackboard&)> cond)
        : condition(std::move(cond)) {
    }

    Status tick(Blackboard& bb) override {
        return condition(bb) ? Status::Success : Status::Failure;
    }

private:
    std::function<bool(Blackboard&)> condition;
};

// Action wrapper
class ActionNode : public Node {
public:
    explicit ActionNode(std::function<Status(Blackboard&)> act)
        : action(std::move(act)) {
    }

    Status tick(Blackboard& bb) override {
        return action(bb);
    }

private:
    std::function<Status(Blackboard&)> action;
};

// simple Tree wrapper
class BehaviorTree {
public:
    explicit BehaviorTree(std::shared_ptr<Node> rootNode)
        : root(std::move(rootNode)) {
    }

    void tick(Blackboard& bb) {
        root->tick(bb);
    }

private:
    std::shared_ptr<Node> root;
};

int main() {
    Blackboard bb;
    bb.setInt("health", 100);
    bb.setInt("playerDistance", 15);
    bb.setInt("chaseProgress", 0);

    auto lowHealth = std::make_shared<ConditionNode>(
        [](Blackboard& bb) {
            return bb.getInt("health") < 30;
        });

    auto playerNear = std::make_shared<ConditionNode>(
        [](Blackboard& bb) {
            return bb.getInt("playerDistance") < 10;
        });

    auto attack = std::make_shared<ActionNode>(
        [](Blackboard&) {
            std::cout << "attacking Player\n";
            return Status::Success;
        });

    auto chase = std::make_shared<ActionNode>(
        [](Blackboard& bb) {
            int progress = bb.getInt("chaseProgress");

            if (progress < 3) {
                std::cout << "chasing step " << progress << "\n";
                bb.setInt("chaseProgress", progress + 1);
                return Status::Running;
            }

            std::cout << "Reached player\n";
            bb.setInt("playerDistance", 5);
            bb.setInt("chaseProgress", 0);
            return Status::Success;
        });

    auto idle = std::make_shared<ActionNode>(
        [](Blackboard&) {
            std::cout << "Idling\n";
            return Status::Success;
        });

    auto attackSeq = std::make_shared<Sequence>();
    attackSeq->addChild(playerNear);
    attackSeq->addChild(attack);

    auto chaseSeq = std::make_shared<Sequence>();
    chaseSeq->addChild(chase);

    auto root = std::make_shared<Selector>();
    root->addChild(lowHealth);
    root->addChild(attackSeq);
    root->addChild(chaseSeq);
    root->addChild(idle);

    BehaviorTree tree(root);

    for (int i = 0; i < 8; ++i) {
        std::cout << "Tick " << i << ":\n";
        tree.tick(bb);
        std::cout << "\n";
    }

    return 0;
}
