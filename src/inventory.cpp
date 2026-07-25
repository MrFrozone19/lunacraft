
#include "inventory.h"

Inventory::Inventory(bool is_creative)
{
    if (is_creative) // Default creative loadout
    {
        inventory[4][0] = {ItemID::aluminum, 1};
        inventory[4][1] = {ItemID::aluminum_ore, 1};
        inventory[4][2] = {ItemID::amethyst_ore, 1};
        inventory[4][3] = {ItemID::beryllium, 1};
        inventory[4][4] = {ItemID::blue_crystal, 1};
        inventory[4][5] = {ItemID::boron_crystal, 1};
        inventory[4][6] = {ItemID::calcite, 1};
        inventory[4][7] = {ItemID::carbon, 1};
        inventory[4][8] = {ItemID::chalchanthite, 1};
        inventory[4][9] = {ItemID::feldspar, 1};

        inventory[3][0] = {ItemID::glass, 1};
        inventory[3][1] = {ItemID::gold_ore, 1};
        inventory[3][2] = {ItemID::granite, 1};
        inventory[3][3] = {ItemID::graphite, 1};
        inventory[3][4] = {ItemID::gravel, 1};
        inventory[3][5] = {ItemID::light, 1};
        inventory[3][6] = {ItemID::magnetite, 1};
        inventory[3][7] = {ItemID::molybdenum_ore, 1};
        inventory[3][8] = {ItemID::moon_bark, 1};
        inventory[3][9] = {ItemID::moon_leaf, 1};

        inventory[2][0] = {ItemID::moon_wood, 1};
        inventory[2][1] = {ItemID::neptunium, 1};
        inventory[2][2] = {ItemID::notchium, 1};
        inventory[2][3] = {ItemID::notchium_ore, 1};
        inventory[2][4] = {ItemID::phosphate, 1};
        inventory[2][5] = {ItemID::polymer, 1};
        inventory[2][6] = {ItemID::quartz_ore, 1};
        inventory[2][7] = {ItemID::rock, 1};
        inventory[2][8] = {ItemID::shale_gravel, 1};
        inventory[2][9] = {ItemID::silver_ore, 1};

        inventory[1][0] = {ItemID::snow, 1};
        inventory[1][1] = {ItemID::zircon_ore, 1};
        inventory[1][2] = {ItemID::sulphur_crystal, 1};
        inventory[1][3] = {ItemID::sulphur_ore, 1};
        inventory[1][4] = {ItemID::titanium, 1};
        inventory[1][5] = {ItemID::titanium_ore, 1};
        inventory[1][6] = {ItemID::topsoil, 1};
        inventory[1][7] = {ItemID::xenostone, 1};
        inventory[1][8] = {ItemID::zircon_ore, 1};
        inventory[1][9] = {ItemID::minilight, 1};

        inventory[0][0] = {ItemID::drill_t3, 1};
        inventory[0][1] = {ItemID::slug_pistol_t3, 1};
        inventory[0][2] = {ItemID::medkit, 1};
        inventory[0][3] = {ItemID::disk, 1};
        inventory[0][4] = {ItemID::chronobooster, 1};
        inventory[0][5] = {ItemID::chronowinder, 1};
        inventory[0][6] = {ItemID::camera, 1};

        spacesuit[0] = {ItemID::jetpack_t3, 1};
        spacesuit[1] = {ItemID::energy_orb, 1};
    }
    else // Default explore loadout
    {
        inventory[0][0] = {ItemID::drill_t1, 1};
        inventory[0][2] = {ItemID::slug_pistol_t1, 1};
        inventory[0][3] = {ItemID::medkit, 1};
        inventory[0][4] = {ItemID::disk, 1};
        inventory[0][9] = {ItemID::camera, 1};

        spacesuit[0] = {ItemID::jetpack_t1, 1};
        spacesuit[1] = {ItemID::battery, 1};
    }

    is_creative_ = is_creative;
}

bool Inventory::HasSpaceForItem(ItemID item)
{
    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            ItemStack slot = inventory[row][col];
            if (slot.item == ItemID::none || slot.amount == 0 || (slot.item == item && ItemIsStackable(item)))
                return true;
        }
    }

    return false;
}

ItemID Inventory::GetSelectedItem()
{
    return inventory[0][selected_hotbar_slot].item;
}

bool Inventory::ItemIsStackable(ItemID item)
{
    return item < ItemID::camera;
}

bool Inventory::IsCreative()
{
    return is_creative_;
}

// Returns index of slot that was added to
int Inventory::Add(ItemStack stack)
{
    //
    // First pass: check for existing stack
    //

    // Hotbar
    for (int col = 0; col < 10; col++)
    {
        ItemStack &slot = inventory[0][col];
        if (slot.item == stack.item)
        {
            slot.amount += stack.amount;
            return col;
        }
    }

    // Inventory
    for (int row = 4; row > 0; row--)
    {
        for (int col = 0; col < 10; col++)
        {
            ItemStack &slot = inventory[row][col];
            if (slot.item == stack.item)
            {
                slot.amount += stack.amount;
                return col + 10*row;
            }
        }
    }

    //
    // Second pass: check for empty slot
    //

    // Hotbar
    for (int col = 0; col < 10; col++)
    {
        ItemStack &slot = inventory[0][col];
        if (slot.IsEmpty())
        {
            slot = stack;
            return col;
        }
    }

    // Inventory
    for (int row = 4; row > 0; row--)
    {
        for (int col = 0; col < 10; col++)
        {
            ItemStack &slot = inventory[row][col];
            if (slot.IsEmpty())
            {
                slot = stack;
                return col + 10*row;
            }
        }
    }

    return -1;
}

std::optional<CraftingRecipe> Inventory::GetRecipeMatch()
{
    auto recipes = GetCraftingRecipes();
    for (auto& recipe : recipes)
    {
        bool match_found = true;

        // Check for correct number of items
        int input_count = 0;
        int recipe_input_count = 0;
        for (int i = 0; i < 9; i++)
        {
            ItemStack& input = assembler_input[i / 3][i % 3];
            if (input.item != ItemID::none)
                input_count++;

            if (i < recipe.input.size() && recipe.input[i].first != ItemID::none)
                recipe_input_count++;
        }

        if (input_count != recipe_input_count)
        {
            match_found = false;
        }
        else
        {
            if (recipe.order_matters)
            {
                bool input_found = false;
                for (int i = 0; i < 9; i++)
                {
                    ItemStack input = assembler_input[i / 3][i % 3];
                    input_found = input_found || input.item != ItemID::none;
                    
                    if (input_found)
                    {
                        int j = 0;
                        while (i < 9 && j < recipe.input.size())
                        {
                            input = assembler_input[i / 3][i % 3];
                            if (input.item != recipe.input[j].first || input.amount < recipe.input[j].second)
                            {
                                match_found = false;
                                break;
                            }
                            i++;
                            j++;
                        }

                        break;
                    }
                }
            }
            else
            {
                // This assumes no input is repeated, which is true for all existing recipes (7/25/26)
                for (auto& [recipe_item, recipe_item_amount] : recipe.input)
                {
                    bool satisfied = false;
                    for (int i = 0; i < 9; i++)
                    {
                        ItemStack input = assembler_input[i / 3][i % 3];
                        if (input.item == recipe_item && input.amount >= recipe_item_amount)
                            satisfied = true;
                    }

                    if (!satisfied)
                    {
                        match_found = false;
                        break;
                    }
                }
            }
        }

        if (match_found)
            return {recipe};
    }

    return std::nullopt;
}

bool ItemStack::IsEmpty()
{
    return item == ItemID::none || amount < 1;
}

bool ItemStack::operator==(const ItemStack &other)
{
    return item == other.item && amount == other.amount;
}

bool ItemStack::operator!=(const ItemStack &other)
{
    return item != other.item || amount != other.amount;
}
