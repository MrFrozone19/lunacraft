#pragma once

#include <vector>

#include "item.h"

struct CraftingRecipe
{
    std::pair<ItemID, int> output;
    std::vector<std::pair<ItemID, int>> input;
    bool order_matters;
};

inline std::vector<CraftingRecipe> GetCraftingRecipes()
{
    static auto recipes = []()
    {
        std::vector<CraftingRecipe> _recipes =
        {
            {
                .output = {ItemID::energy_orb, 1},
                .input = {{ItemID::none, 0},          {ItemID::power_crystal, 1}, {ItemID::none, 0},
                {ItemID::power_crystal, 1}, {ItemID::neptunium, 1},     {ItemID::power_crystal, 1},
                {ItemID::none, 0},          {ItemID::power_crystal, 1}, {ItemID::none, 0}},
                .order_matters = true
            },
            {
                .output = {ItemID::power_crystal, 1},
                .input = {{ItemID::boron_crystal, 1},   {ItemID::sulphur_crystal, 1}, {ItemID::boron_crystal, 1},
                {ItemID::sulphur_crystal, 1}, {ItemID::boron_crystal, 1},   {ItemID::sulphur_crystal, 1},
                {ItemID::boron_crystal, 1},   {ItemID::sulphur_crystal, 1}, {ItemID::boron_crystal, 1}},
                .order_matters = true
            },
            {
                .output = {ItemID::battery, 1},
                .input = {{ItemID::aluminum, 1}, {ItemID::aluminum, 1}, {ItemID::aluminum, 1},
                {ItemID::light, 1},    {ItemID::light, 1},    {ItemID::light, 1},
                {ItemID::aluminum, 1}, {ItemID::aluminum, 1}, {ItemID::aluminum, 1}},
                .order_matters = true
            },
            {
                .output = {ItemID::beacon, 1},
                .input = {{ItemID::aluminum, 1}, {ItemID::chronobooster, 1},   {ItemID::aluminum, 1},
                {ItemID::aluminum, 1}, {ItemID::power_crystal, 1}, {ItemID::aluminum, 1},
                {ItemID::aluminum, 1}, {ItemID::mechanism, 1},     {ItemID::aluminum, 1}},
                .order_matters = true
            },
            {
                .output = {ItemID::mechanism, 1},
                .input = {{ItemID::aluminum, 1}, {ItemID::polymer, 1},  {ItemID::aluminum, 1},
                {ItemID::polymer, 1},  {ItemID::aluminum, 1}, {ItemID::polymer, 1},
                {ItemID::aluminum, 1}, {ItemID::polymer, 1},  {ItemID::aluminum, 1}},
                .order_matters = true
            },
            {
                .output = {ItemID::drill_t1, 1},
                .input = {{ItemID::mechanism, 1}, {ItemID::none, 0},     {ItemID::none, 0},
                {ItemID::battery, 1},   {ItemID::aluminum, 1}, {ItemID::aluminum, 1},
                {ItemID::mechanism, 1}, {ItemID::none, 0},     {ItemID::none, 0}},
                .order_matters = true
            },
            {
                .output = {ItemID::drill_t2, 1},
                .input = {{ItemID::mechanism, 1},     {ItemID::none, 0},     {ItemID::none, 0},
                {ItemID::power_crystal, 1}, {ItemID::titanium, 1}, {ItemID::titanium, 1},
                {ItemID::mechanism, 1},     {ItemID::none, 0},     {ItemID::none, 0}},
                .order_matters = true
            },
            {
                .output = {ItemID::drill_t3, 1},
                .input = {{ItemID::mechanism, 1},  {ItemID::none, 0},     {ItemID::none, 0},
                {ItemID::energy_orb, 1}, {ItemID::notchium, 1}, {ItemID::notchium, 1},
                {ItemID::mechanism, 1},  {ItemID::none, 0},     {ItemID::none, 0}},
                .order_matters = true
            },
            {
                .output = {ItemID::slug_pistol_t1, 1},
                .input = {{ItemID::battery, 1}, {ItemID::magnet, 1}, {ItemID::aluminum, 1},
                {ItemID::aluminum, 1}},
                .order_matters = true
            },
            {
                .output = {ItemID::slug_pistol_t2, 1},
                .input = {{ItemID::power_crystal, 1}, {ItemID::magnet, 1}, {ItemID::titanium, 1},
                {ItemID::titanium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::slug_pistol_t3, 1},
                .input = {{ItemID::energy_orb, 1}, {ItemID::magnet, 1}, {ItemID::notchium, 1},
                {ItemID::notchium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::jetpack_t1, 1},
                .input = {{ItemID::aluminum, 1}, {ItemID::aluminum, 1}, {ItemID::aluminum, 1},
                {ItemID::battery, 1},  {ItemID::battery, 1},  {ItemID::battery, 1},
                {ItemID::aluminum, 1}, {ItemID::none, 0},     {ItemID::aluminum, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::jetpack_t2, 1},
                .input = {{ItemID::titanium, 1},      {ItemID::titanium, 1},      {ItemID::titanium, 1},
                {ItemID::power_crystal, 1}, {ItemID::power_crystal, 1}, {ItemID::power_crystal, 1},
                {ItemID::titanium, 1},      {ItemID::none, 0},          {ItemID::titanium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::jetpack_t3, 1},
                .input = {{ItemID::notchium, 1},   {ItemID::notchium, 1},   {ItemID::notchium, 1},
                {ItemID::energy_orb, 1}, {ItemID::energy_orb, 1}, {ItemID::energy_orb, 1},
                {ItemID::notchium, 1},   {ItemID::none, 0},       {ItemID::notchium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::camera, 1},
                .input = {{ItemID::none, 0},   {ItemID::energy_orb, 1}, {ItemID::none, 0},
                {ItemID::carbon, 1}, {ItemID::mechanism, 1}, {ItemID::carbon, 1},
                {ItemID::carbon, 1}, {ItemID::mechanism, 1}, {ItemID::carbon, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::medkit, 1},
                .input = {{ItemID::aluminum, 1}, {ItemID::mechanism, 1}, {ItemID::aluminum, 1},
                {ItemID::aluminum, 1}, {ItemID::biogel, 6},    {ItemID::aluminum, 1},
                {ItemID::aluminum, 1}, {ItemID::aluminum, 1},  {ItemID::aluminum, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::turret_t1, 1},
                .input = {{ItemID::none, 0},     {ItemID::slug_pistol_t1, 1}, {ItemID::none, 0},
                {ItemID::titanium, 1}, {ItemID::battery, 1},        {ItemID::titanium, 1},
                {ItemID::titanium, 1}, {ItemID::battery, 1},        {ItemID::titanium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::turret_t2, 1},
                .input = {{ItemID::none, 0},    {ItemID::slug_pistol_t2, 1},   {ItemID::none, 0},
                {ItemID::polymer, 1}, {ItemID::power_crystal, 1},    {ItemID::polymer, 1},
                {ItemID::polymer, 1}, {ItemID::power_crystal, 1},    {ItemID::polymer, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::turret_t3, 1},
                .input = {{ItemID::none, 0},     {ItemID::slug_pistol_t3, 1}, {ItemID::none, 0},
                {ItemID::notchium, 1}, {ItemID::energy_orb, 1},     {ItemID::notchium, 1},
                {ItemID::notchium, 1}, {ItemID::energy_orb, 1},     {ItemID::notchium, 1}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::chronobooster, 1},
                .input = {{ItemID::none, 0},      {ItemID::xenostone, 1}, {ItemID::none, 0},
                {ItemID::xenostone, 1}, {ItemID::mechanism, 1}, {ItemID::xenostone, 1},
                {ItemID::none, 0},      {ItemID::xenostone, 1}, {ItemID::none, 0}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::chronowinder, 1},
                .input = {{ItemID::none, 0},         {ItemID::amethyst_ore, 8}, {ItemID::none, 0},
                {ItemID::amethyst_ore, 8}, {ItemID::mechanism, 1},    {ItemID::amethyst_ore, 8},
                {ItemID::none, 0},         {ItemID::amethyst_ore, 8}, {ItemID::none, 0}},
                .order_matters = true
            },
            
            {
                .output = {ItemID::minilight, 4},
                .input = {{ItemID::light, 1}, {ItemID::adhesive, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::granite, 1},
                .input = {{ItemID::sulphur_ore, 1}, {ItemID::feldspar, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::granite, 1},
                .input = {{ItemID::feldspar, 1}, {ItemID::sulphur_ore, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::beryllium, 1},
                .input = {{ItemID::sulphur_ore, 1}, {ItemID::chalchanthite, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::beryllium, 1},
                .input = {{ItemID::chalchanthite, 1}, {ItemID::sulphur_ore, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::amethyst_ore, 1},
                .input = {{ItemID::chalchanthite, 1}, {ItemID::feldspar, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::amethyst_ore, 1},
                .input = {{ItemID::feldspar, 1}, {ItemID::chalchanthite, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::rock, 1},
                .input = {{ItemID::calcite, 1}, {ItemID::graphite, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::rock, 1},
                .input = {{ItemID::graphite, 1}, {ItemID::calcite, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::graphite, 1},
                .input = {{ItemID::moon_bark, 1}, {ItemID::rock, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::graphite, 1},
                .input = {{ItemID::rock, 1}, {ItemID::moon_bark, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::calcite, 1},
                .input = {{ItemID::moon_bark, 1}, {ItemID::sand, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::calcite, 1},
                .input = {{ItemID::sand, 1}, {ItemID::moon_bark, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::biogel, 10},
                .input = {{ItemID::xenostone, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::xenostone, 1},
                .input = {{ItemID::biogel, 10}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::sand, 2},
                .input = {{ItemID::dirt, 1}, {ItemID::water, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::sand, 2},
                .input = {{ItemID::water, 1}, {ItemID::dirt, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::adhesive, 3},
                .input = {{ItemID::biogel, 1}, {ItemID::water, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::adhesive, 3},
                .input = {{ItemID::water, 1}, {ItemID::biogel, 1}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::magnet, 1},
                .input = {{ItemID::magnetite, 4}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::polymer, 1},
                .input = {{ItemID::shale_gravel, 4}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::gold, 1},
                .input = {{ItemID::gold_ore, 4}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::aluminum, 1},
                .input = {{ItemID::aluminum_ore, 4}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::titanium, 1},
                .input = {{ItemID::titanium_ore, 4}},
                .order_matters = false
            },
            
            {
                .output = {ItemID::notchium, 1},
                .input = {{ItemID::notchium_ore, 4}},
                .order_matters = false
            },
            {
                .output = {ItemID::biogel, 1},
                .input = {{ItemID::moon_leaf, 8}},
                .order_matters = false
            },
            {
                .output = {ItemID::neptunium, 1},
                .input = {{ItemID::blue_crystal, 16}},
                .order_matters = false
            }
        };

        return _recipes;
    }();

    return recipes;
};
