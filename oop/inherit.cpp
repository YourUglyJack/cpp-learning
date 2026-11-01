#include <iostream>
#include <string>
#include "../common/myutils.h"

class Character
{
public:
    Character(std::string name, int hp)
        : name_(name), hp_(hp)
    {
        std::cout << "[Character] " << name_ << " create with HP=" << hp_ << "\n";
    }

    ~Character()
    {
        std::cout << "[Character] " << name_ << " destroyed\n";
    }

    void take_damage(int dmg)
    {
        hp_ -= dmg;
        std::cout << name_ << " takes " << dmg << " damge, HP=" << hp_ << "\n";
    }

    void heal(int amount)
    {
        hp_ += amount;
        std::cout << name_ << " heals " << amount << ", HP=" << hp_ << "\n";
    }

    std::string name() const { return name_; }
    int hp() const { return hp_; }

    void status() const
    {
        std::cout << "[Status] " << name_ << " HP=" << hp_ << "\n";
    }

protected:
    std::string name_;
    int hp_;
};

class Warrior : public Character
{
    // public 继承, 基类的 public 还是 public， protected 还是 protected
public:
    Warrior(std::string name)
        : Character(name, 100), armor_(20)
    {
        std::cout << "[Warrior] " << name_ << " equipped armor=" << armor_ << "\n";
    }

    ~Warrior()
    {
        std::cout << "[Warrior] " << name_ << "unequipped armor\n";
    }

    void slash()
    {
        int damage = 30;
        std::cout << name_ << " slashes! " << damage << " damage\n";
    }

    void block()
    {
        std::cout << name_ << " blocks with shield! Armor=" << armor_ << "\n";
    }

    // 没有 override，会根据变量类型 看 调用 base class 的 还是 sub class 的
    void take_damage(int dmg)
    {
        int actual = dmg - armor_ / 2;
        if (actual < 0)
            actual = 0;
        hp_ -= actual;
        std::cout << name_ << " takes " << actual << " damage (blocked "
                  << (dmg - actual) << "), HP=" << hp_ << "\n";
    }

private:
    int armor_;
};

class Mage : public Character
{
public:
    Mage(std::string name)
        : Character(name, 60), mana_(100)
    {
        std::cout << "[Mage] " << name_ << " obtained spellbook, mana=" << mana_ << "\n";
    }
    ~Mage()
    {
        std::cout << "[Mage] " << name_ << " spellbook vanished\n";
    }

    // 火球术
    void fireball()
    {
        int cost = 20;
        if (mana_ >= cost)
        {
            mana_ -= cost;
            std::cout << name_ << " casts Fireball! 50 damage, mana=" << mana_ << "\n";
        }
        else
        {
            std::cout << name_ << " not enough mana! need " << cost << ", have " << mana_ << "\n";
        }
    }

    // 冥想回蓝
    void meditate()
    {
        int restore = 30;
        mana_ += restore;
        std::cout << name_ << " meditates, mana=" << mana_ << "\n";
    }

    // 显示法师状态（包含蓝量）
    void status() const
    {
        std::cout << "[Status] " << name_ << " HP=" << hp_ << " Mana=" << mana_ << "\n";
    }

private:
    int mana_;
};

int main() {
    print_title("create chracter");
    // create: base class, subclass
    // destroy: sub class, base class
    Warrior w{"Arthur"};
    Mage m("Merliin");

    print_title("status");
    w.status();
    m.status();

    print_title("take dmg");
    w.take_damage(50);      // Warrior 版本（有护甲减伤）
    m.take_damage(30);      // 继承自 Character（无减伤）
    
    Character& ref = w;
    ref.take_damage(10); // 因为没有 override 所以根据 variavle 类型调用

    print_title("game over");
}