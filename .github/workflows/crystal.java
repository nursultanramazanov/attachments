name: Crystal CI

on: import cv2
import numpy as np
from tqdm import tqdm

# Define the available resolutions
resolutions = [
    (320, 240),
    (640, 480),
    (1280, 720),
    (1920, 1080),
    (750, 1334),   # iPhone 6/7/8
    (1080, 1920),  # iPhone 6/7/8 Plus
    (1125, 2436),  # iPhone X/XS/11 Pro
    (1242, 2688),  # iPhone XS Max/11 Pro Max
    (1440, 2560),  # Google Pixel 2 XL
    (1440, 3040),  # Samsung Galaxy S10/S20
    (1440, 3200),  # Sony Xperia 1 II
    (1600, 2560),  # Google Pixel 4 XL
    (2160, 3840)   # Samsung Galaxy S21 Ultra
]

# Print the available resolutions
print("Available resolutions:")
for i, res in enumerate(resolutions):
    if i == 4:
        print("\nFor smartphones:")
    print(f"{i+1}. {res[0]}x{res[1]}")
print()

# Ask the user to select or enter a resolution
res_choice = input("Enter the number of your choice or enter a custom resolution (e.g. 1920x1080): ")
if "x" in res_choice:
    resolution = tuple(map(int, res_choice.split("x")))
else:
    res_choice = int(res_choice)
    resolution = resolutions[res_choice-1]

# Define the video codec and frame rate
codec = cv2.VideoWriter_fourcc(*"mp4v")
fps = 60

# Create the video writer object
filename = f"static_{resolution[0]}x{resolution[1]}.mp4"
out = cv2.VideoWriter(filename, codec, fps, resolution)

# Generate and write each frame of the video
for i in tqdm(range(fps * 10)):
    frame = np.random.randint(0, 255, (resolution[1], resolution[0], 3), dtype=np.uint8)
    out.write(frame)

# Release the video writer object
out.release()

print(f"Static video saved to {filename}")
  push: import numpy as np
from PIL import Image
from tqdm import tqdm
import imageio

# Set up variables
resolution = input("Enter the resolution (e.g. 1920x1080): ")
fps = int(input("Enter the frames per second (e.g. 60): "))
duration = int(input("Enter the duration in seconds (e.g. 10): "))

# Split resolution into width and height
width, height = map(int, resolution.split("x"))

# Generate static frames
frames = []
for i in tqdm(range(int(fps * duration)), desc="Generating frames"):
    frame = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)
    frames.append(Image.fromarray(frame))

# Save frames as GIF
filename = f"static_{resolution}.gif"
imageio.mimsave(filename, frames, fps=fps)
print(f"Static saved as {filename}")
    branches: [ "main" ]
  pull_request: import cv2
import numpy as np
from tqdm import tqdm

# Set the width and height of the video
width = 1920
height = 1080

# Set the frame rate of the video
fps = 60

# Create a VideoWriter object to write the output video
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter('static.mp4', fourcc, fps, (width, height))

# Generate frames of static and write them to the output video
for i in tqdm(range(1800)):
    # Create a random matrix of values between 0 and 255
    static = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)

    # Apply a Gaussian blur to the matrix to simulate the blurring effect
    static = cv2.GaussianBlur(static, (3, 3), 0)

    # Write the frame to the output video
    out.write(static)

# Release the VideoWriter object
out.release()

    branches: [ "main" ]

jobs: distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-8.4-bin.zip
networkTimeout=10000
validateDistributionUrl=true
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
  build: package io.savagedev.savagecore.init;

/*
 * ModConfigs.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import net.minecraftforge.common.ForgeConfigSpec;

public class ModConfigs
{
    public static final ForgeConfigSpec COMMON;

    public static final ForgeConfigSpec.BooleanValue ENABLE_UPDATE_CHECK;

    static {
        final ForgeConfigSpec.Builder common = new ForgeConfigSpec.Builder();

        common.comment("General configuration options.").push("General");

        ENABLE_UPDATE_CHECK = common.comment("Enable update checking with in-game display").translation("configGui.savagecore.enable_update_check").define("enableUpdateCheck", true);

        common.pop();

        COMMON = common.build();
    }
}

    runs-on: package io.savagedev.savagecore.item;

/*
 * BaseItemStackHandler.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import net.minecraft.core.NonNullList;
import net.minecraft.world.item.ItemStack;
import net.minecraftforge.items.ItemStackHandler;
import org.apache.commons.lang3.ArrayUtils;

import java.util.HashMap;
import java.util.Map;
import java.util.function.BiFunction;

public class BaseItemStackHandler extends ItemStackHandler
{
    private final Runnable onContentsChanged;
    private final Map<Integer, Integer> slotSizeMap;
    private BiFunction<Integer, ItemStack, Boolean> slotValidator = null;
    private int maxStackSize = 64;
    private int[] outputSlots = null;

    public BaseItemStackHandler(int size) {
        this(size, null);
    }

    public BaseItemStackHandler(int size, Runnable onContentsChanged) {
        super(size);
        this.onContentsChanged = onContentsChanged;
        this.slotSizeMap = new HashMap<>();
    }

    @Override
    public ItemStack insertItem(int slot, ItemStack stack, boolean simulate) {
        if (this.outputSlots != null && ArrayUtils.contains(this.outputSlots, slot))
            return stack;
        return super.insertItem(slot, stack, simulate);
    }

    @Override
    public ItemStack extractItem(int slot, int amount, boolean simulate) {
        if (this.outputSlots != null && !ArrayUtils.contains(this.outputSlots, slot))
            return ItemStack.EMPTY;
        return super.extractItem(slot, amount, simulate);
    }

    @Override
    public int getSlotLimit(int slot) {
        return this.slotSizeMap.containsKey(slot) ? this.slotSizeMap.get(slot) : this.maxStackSize;
    }

    @Override
    public boolean isItemValid(int slot, ItemStack stack) {
        return this.slotValidator == null || this.slotValidator.apply(slot, stack);
    }

    @Override
    protected void onContentsChanged(int slot) {
        if (this.onContentsChanged != null)
            this.onContentsChanged.run();
    }

    public ItemStack insertItemSuper(int slot, ItemStack stack, boolean simulate) {
        return super.insertItem(slot, stack, simulate);
    }

    public ItemStack extractItemSuper(int slot, int amount, boolean simulate) {
        return super.extractItem(slot, amount, simulate);
    }

    public NonNullList<ItemStack> getStacks() {
        return this.stacks;
    }

    public int[] getOutputSlots() {
        return this.outputSlots;
    }

    public void setDefaultSlotLimit(int size) {
        this.maxStackSize = size;
    }

    public void addSlotLimit(int slot, int size) {
        this.slotSizeMap.put(slot, size);
    }

    public void setSlotValidator(BiFunction<Integer, ItemStack, Boolean> validator) {
        this.slotValidator = validator;
    }

    public void setOutputSlots(int... slots) {
        this.outputSlots = slots;
    }
}

    container: package io.savagedev.savagecore.item;

/*
 * ItemHelper.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import com.google.common.collect.ImmutableList;
import net.minecraft.client.Minecraft;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.Vec3i;
import net.minecraft.network.protocol.game.ClientboundBlockUpdatePacket;
import net.minecraft.network.protocol.game.ServerboundPlayerActionPacket;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.util.Mth;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.ClipContext;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.block.Block;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.phys.BlockHitResult;
import net.minecraft.world.phys.HitResult;
import net.minecraft.world.phys.Vec3;
import net.minecraftforge.common.ForgeHooks;
import net.minecraftforge.common.ForgeMod;
import net.minecraftforge.event.ForgeEventFactory;

public class ItemHelper
{
    public static ImmutableList<BlockPos> calcAOEBlocks(ItemStack stack, Level world, Player player, BlockPos origin, int width, int height, int depth) {
        return calcAOEBlocks(stack, world, player, origin, width, height, depth, -1);
    }

    public static ImmutableList<BlockPos> calcAOEBlocks(ItemStack stack, Level world, Player player, BlockPos origin, int width, int height, int depth, int distance) {
        if (stack == null)
            return ImmutableList.of();

        BlockState state = world.getBlockState(origin);

        if (state.isAir()) {
            return ImmutableList.of();
        }

        HitResult mop = rayTrace(world, player, ClipContext.Fluid.ANY);
        if (mop == null) {
            return ImmutableList.of();
        }

        int x, y, z;
        BlockPos start = origin;

        if(mop.getType() == HitResult.Type.BLOCK) {
            BlockHitResult blockRayTraceResult = (BlockHitResult) mop;

            if(!origin.equals(blockRayTraceResult.getBlockPos())) {
                return ImmutableList.of();
            }

            switch (blockRayTraceResult.getDirection()) {
                case DOWN:
                case UP:
                    Vec3i vec = player.getDirection().getNormal();
                    x = vec.getX() * height + vec.getZ() * width;
                    y = blockRayTraceResult.getDirection().getAxisDirection().getStep() * -depth;
                    z = vec.getX() * width + vec.getZ() * height;
                    start = start.offset(-x / 2, 0, -z / 2);
                    if (x % 2 == 0) {
                        if (x > 0 && blockRayTraceResult.getLocation().x() - blockRayTraceResult.getBlockPos().getX() > 0.5d)
                            start = start.offset(1, 0, 0);
                        else if (x < 0 && blockRayTraceResult.getLocation().x() - blockRayTraceResult.getBlockPos().getX() < 0.5d)
                            start = start.offset(-1, 0, 0);
                    }
                    if (z % 2 == 0) {
                        if (z > 0 && blockRayTraceResult.getLocation().z() - blockRayTraceResult.getBlockPos().getZ() > 0.5d)
                            start = start.offset(0, 0, 1);
                        else if (z < 0 && blockRayTraceResult.getLocation().z() - blockRayTraceResult.getBlockPos().getZ() < 0.5d)
                            start = start.offset(0, 0, -1);
                    }
                    break;
                case NORTH:
                case SOUTH:
                    x = width;
                    y = height;
                    z = blockRayTraceResult.getDirection().getAxisDirection().getStep() * -depth;
                    start = start.offset(-x / 2, -y / 2, 0);
                    if (x % 2 == 0 && blockRayTraceResult.getLocation().x() - blockRayTraceResult.getBlockPos().getX() > 0.5d)
                        start = start.offset(1, 0, 0);
                    if (y % 2 == 0 && blockRayTraceResult.getLocation().y() - blockRayTraceResult.getBlockPos().getY() > 0.5d)
                        start = start.offset(0, 1, 0);
                    break;
                case WEST:
                case EAST:
                    x = blockRayTraceResult.getDirection().getAxisDirection().getStep() * -depth;
                    y = height;
                    z = width;
                    start = start.offset(-0, -y / 2, -z / 2);
                    if (y % 2 == 0 && blockRayTraceResult.getLocation().y() - blockRayTraceResult.getBlockPos().getY() > 0.5d)
                        start = start.offset(0, 1, 0);
                    if (z % 2 == 0 && blockRayTraceResult.getLocation().z() - blockRayTraceResult.getBlockPos().getZ() > 0.5d)
                        start = start.offset(0, 0, 1);
                    break;
                default:
                    x = y = z = 0;
            }

            ImmutableList.Builder<BlockPos> builder = ImmutableList.builder();
            for (int xp = start.getX(); xp != start.getX() + x; xp += x / Mth.abs(x)) {
                for (int yp = start.getY(); yp != start.getY() + y; yp += y / Mth.abs(y)) {
                    for (int zp = start.getZ(); zp != start.getZ() + z; zp += z / Mth.abs(z)) {
                        if (xp == origin.getX() && yp == origin.getY() && zp == origin.getZ()) {
                            continue;
                        }

                        if (distance > 0 && Mth.abs(xp - origin.getX()) + Mth.abs(yp - origin.getY()) + Mth.abs(
                                zp - origin.getZ()) > distance) {
                            continue;
                        }

                        BlockPos pos = new BlockPos(xp, yp, zp);
                        if (stack.getItem().isCorrectToolForDrops(world.getBlockState(pos))) {
                            builder.add(pos);
                        }
                    }
                }
            }

            return builder.build();
        }

        return ImmutableList.of();
    }

    public static void destroyExtraAOEBlocks(ItemStack stack, Level world, Player player, BlockPos pos, BlockPos refPos) {
        if (world.getBlockState(pos).isAir())
            return;

        BlockState state = world.getBlockState(pos);
        Block block = state.getBlock();

        if (!stack.getItem().isCorrectToolForDrops(state)) {
            return;
        }

        BlockState refState = world.getBlockState(refPos);
        float refStrength = blockStrength(refState, player, world, refPos);
        float strength = blockStrength(state, player, world, pos);

        if (!ForgeHooks.isCorrectToolForDrops(state, player) || refStrength / strength > 10f)
            return;

        if (player.isCreative()) {
            block.playerWillDestroy(world, pos, state, player);
            if(block.onDestroyedByPlayer(state, world, pos, player, false, state.getFluidState()))
                block.destroy(world, pos, state);

            if (!world.isClientSide()) {
                ((ServerPlayer) player).connection.send(new ClientboundBlockUpdatePacket(world, pos));
            }

            return;
        }

        stack.mineBlock(world, state, pos, player);

        if (!world.isClientSide()) {
            int xp = ForgeHooks.onBlockBreakEvent(world, ((ServerPlayer) player).gameMode.getGameModeForPlayer(), (ServerPlayer) player, pos);

            if (xp == -1) {
                return;
            }

            block.playerWillDestroy(world, pos, state, player);

            if (block.onDestroyedByPlayer(state, world, pos, player, true, state.getFluidState())) {
                block.destroy(world, pos, state);
                block.playerDestroy(world, player, pos, state, world.getBlockEntity(pos), stack);
                block.popExperience((ServerLevel) world, pos, xp);
            }

            ServerPlayer mpPlayer = (ServerPlayer) player;
            mpPlayer.connection.send(new ClientboundBlockUpdatePacket(world, pos));
        } else {
            world.globalLevelEvent(2001, pos, Block.getId(state));

            if (block.onDestroyedByPlayer(state, world, pos, player, true, state.getFluidState())) {
                block.destroy(world, pos, state);
            }

            stack.mineBlock(world, state, pos, player);

            if (stack.getMaxStackSize() == 0 && stack == player.getMainHandItem()) {
                ForgeEventFactory.onPlayerDestroyItem(player, stack, InteractionHand.MAIN_HAND);
                player.setItemInHand(InteractionHand.MAIN_HAND, ItemStack.EMPTY);
            }

            Minecraft.getInstance().getConnection().send(new ServerboundPlayerActionPacket(ServerboundPlayerActionPacket.Action.STOP_DESTROY_BLOCK, pos, Direction.DOWN));
        }
    }

    public static HitResult rayTrace(Level worldIn, Player player, ClipContext.Fluid fluidMode) {
        float f = player.getXRot();
        float f1 = player.getYRot();
        Vec3 vec3d = player.getEyePosition();
        float f2 = Mth.cos(-f1 * ((float)Math.PI / 180F) - (float)Math.PI);
        float f3 = Mth.sin(-f1 * ((float)Math.PI / 180F) - (float)Math.PI);
        float f4 = -Mth.cos(-f * ((float)Math.PI / 180F));
        float f5 = Mth.sin(-f * ((float)Math.PI / 180F));
        float f6 = f3 * f4;
        float f7 = f2 * f4;
        double d0 = player.getAttribute(ForgeMod.BLOCK_REACH.get()).getValue();
        Vec3 vec3d1 = vec3d.add((double)f6 * d0, (double)f5 * d0, (double)f7 * d0);
        return worldIn.clip(new ClipContext(vec3d, vec3d1, ClipContext.Block.OUTLINE, fluidMode, player));
    }

    public static float blockStrength(BlockState state, Player player, Level world, BlockPos pos) {
        float hardness = state.getDestroySpeed(world, pos);
        if (hardness < 0.0F) {
            return 0.0F;
        }

        if (!ForgeHooks.isCorrectToolForDrops(state, player)) {
            return player.getDigSpeed(state, pos) / hardness / 100F;
        } else {
            return player.getDigSpeed(state, pos) / hardness / 30F;
        }
    }

    public static boolean equalsIgnoreStackSize(ItemStack stack1, ItemStack stack2) {
        if(stack1 != null && stack2 != null) {
            if(Item.getId(stack1.getItem().asItem()) - Item.getId(stack2.getItem()) == 0) {
                if(stack1.getItem() == stack2.getItem()) {
                    if(stack1.getDamageValue() == stack2.getDamageValue()) {
                        if(stack1.hasTag() && stack2.hasTag()) {
                            return ItemStack.isSameItem(stack1, stack2);
                        } else {
                            return false;
                        }
                    }
                }
            }
        }

        return false;
    }
}
      image: package io.savagedev.savagecore.nbt;

/*
 * NBTHelper.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import net.minecraft.nbt.CompoundTag;
import net.minecraft.world.item.ItemStack;

public class NBTHelper
{
    private static void initNBTTagCompound(ItemStack itemStack) {
        if (itemStack.getTag() == null) {
            itemStack.setTag(new CompoundTag());
        }
    }

    public static boolean hasTag(ItemStack itemStack, String keyName) {
        return itemStack.hasTag() && itemStack.getTag().contains(keyName);
    }

    public static int getInt(ItemStack itemStack, String keyName) {
        initNBTTagCompound(itemStack);

        if (!itemStack.getTag().contains(keyName)) {
            setInt(itemStack, keyName, 0);
        }

        return itemStack.getTag().getInt(keyName);
    }

    public static void setInt(ItemStack itemStack, String keyName, int keyValue) {
        initNBTTagCompound(itemStack);

        itemStack.getTag().putInt(keyName, keyValue);
    }

    public static String getString(ItemStack itemStack, String keyName) {
        initNBTTagCompound(itemStack);

        if (!itemStack.getTag().contains(keyName)) {
            setString(itemStack, keyName, "");
        }

        return itemStack.getTag().getString(keyName);
    }

    public static void setString(ItemStack itemStack, String keyName, String keyValue) {
        initNBTTagCompound(itemStack);

        itemStack.getTag().putString(keyName, keyValue);
    }

    public static long getLong(ItemStack itemStack, String keyName) {
        initNBTTagCompound(itemStack);

        if (!itemStack.getTag().contains(keyName)) {
            setLong(itemStack, keyName, 0L);
        }

        return itemStack.getTag().getLong(keyName);
    }

    public static void setLong(ItemStack itemStack, String keyName, long keyValue) {
        initNBTTagCompound(itemStack);

        itemStack.getTag().putLong(keyName, keyValue);
    }
}

    steps: package io.savagedev.savagecore.util.logger;

/*
 * LogHelper.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import io.savagedev.savagecore.util.reference.CoreReference;
import org.apache.logging.log4j.*;
import org.apache.logging.log4j.message.Message;

public class LogHelper
{
    private static final Logger logger = LogManager.getLogger(CoreReference.MOD_ID);

    public static final Marker MOD_MARKER = MarkerManager.getMarker(CoreReference.MOD_ID);

    public static final String TAG = "[" + CoreReference.MOD_NAME+ "] ";

    public static void log(Level level, Marker marker, Message message) {
        logger.log(level, marker, TAG + message);
    }
    public static void log(Level level, Marker marker, Object object) {
        logger.log(level, marker, TAG + object);
    }

    public static void log(Level level, Marker marker, String message) {
        logger.log(level, marker, TAG + message);
    }

    public static void log(Level level, Marker marker, String format, Object... params) {
        logger.log(level, marker, TAG + format, params);
    }

    public static void log(Level level, Message message) {
        log(level, MOD_MARKER, message);
    }

    public static void log(Level level, Object object) {
        log(level, MOD_MARKER, object);
    }

    public static void log(Level level, String message) {
        log(level, MOD_MARKER, message);
    }

    public static void log(Level level, String format, Object... params) {
        log(level, MOD_MARKER, format, params);
    }

    //Info Logger

    public static void info(Marker marker, Message message) {
        log(Level.INFO, marker, message);
    }

    public static void info(Marker marker, Object object) {
        log(Level.INFO, marker, object);
    }

    public static void info(Marker marker, String message) {
        log(Level.INFO, marker, message);
    }

    public static void info(Marker marker, String format, Object... params) {
        log(Level.INFO, marker, format, params);
    }

    public static void info(Message message) {
        info(MOD_MARKER, message);
    }

    public static void info(Object object) {
        info(MOD_MARKER, object);
    }

    public static void info(String message) {
        info(MOD_MARKER, message);
    }

    public static void info(String format, Object... params) {
        info(MOD_MARKER, format, params);
    }

    // Warning Logger

    public static void warn(Marker marker, Message message) {
        log(Level.WARN, marker, message);
    }

    public static void warn(Marker marker, Object object) {
        log(Level.WARN, marker, object);
    }

    public static void warn(Marker marker, String message) {
        log(Level.WARN, marker, message);
    }

    public static void warn(Marker marker, String format, Object... params) {
        log(Level.WARN, marker, format, params);
    }

    public static void warn(Message message) {
        warn(MOD_MARKER, message);
    }

    public static void warn(Object object) {
        warn(MOD_MARKER, object);
    }

    public static void warn(String message) {
        warn(MOD_MARKER, message);
    }

    public static void warn(String format, Object... params) {
        warn(MOD_MARKER, format, params);
    }

    // Error Logger

    public static void error(Marker marker, Message message) {
        log(Level.ERROR, marker, message);
    }

    public static void error(Marker marker, Object object) {
        log(Level.ERROR, marker, object);
    }

    public static void error(Marker marker, String message) {
        log(Level.ERROR, marker, message);
    }

    public static void error(Marker marker, String format, Object... params) {
        log(Level.ERROR, marker, format, params);
    }

    public static void error(Message message) {
        error(MOD_MARKER, message);
    }

    public static void error(Object object) {
        error(MOD_MARKER, object);
    }

    public static void error(String message) {
        error(MOD_MARKER, message);
    }

    public static void error(String format, Object... params) {
        error(MOD_MARKER, format, params);
    }

    // Debug Logger

    public static void debug(Marker marker, Message message) {
        log(Level.DEBUG, marker, message);
    }

    public static void debug(Marker marker, Object object) {
        log(Level.DEBUG, marker, object);
    }

    public static void debug(Marker marker, String message) {
        log(Level.DEBUG, marker, message);
    }

    public static void debug(Marker marker, String format, Object... params) {
        log(Level.DEBUG, marker, format, params);
    }

    public static void debug(Message message) {
        debug(MOD_MARKER, message);
    }

    public static void debug(Object object) {
        debug(MOD_MARKER, object);
    }

    public static void debug(String message) {
        debug(MOD_MARKER, message);
    }

    public static void debug(String format, Object... params) {
        debug(MOD_MARKER, format, params);
    }
}
    - uses: package io.savagedev.savagecore.util.reference;

/*
 * CoreReference.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

public class CoreReference
{
    public static final String MOD_ID = "savagecore";
    public static final String MOD_NAME = "SavageCore";

    public static class Updater
    {
        public static final String UPDATE_URL = "https://api.savagedev.io";
        public static final String BEGIN_UPDATE_CHECK = "Starting update check for {}...";
        public static final String UPDATE_CHECK = "Checking for update...";
        public static final String UP_TO_DATE = "Up to date.";
        public static final String OUTDATED = "This version is outdated! Newest version: {}";
        public static final String UPDATE_STATUS = "Updater status: {}";
        public static final String UPDATE_FAILED = "Update check failed!";
        public static final String UPDATE_DISABLED = "Update checking is disabled! Change this in the config file.";
        public static final String UNSUPPORTED = "This version of Minecraft may not be supported (yet).";
    }

    public static class Strings
    {
        public static final String DOWNLOAD = "Download";
        public static final String RECOMMENDED = "Recommended";
        public static final String LATEST = "Latest";
    }
}
    - name: package io.savagedev.savagecore.util.updater;

/*
 * UpdateStatus.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

public enum UpdateStatus
{
    NONE,
    PENDING,
    UP_TO_DATE,
    OUTDATED,
    AHEAD,
    UNSUPPORTED,
    UNKNOWN
}
      run: package io.savagedev.savagecore.util.updater;

/*
 * Updater.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import io.savagedev.savagecore.util.reference.CoreReference;
import io.savagedev.savagecore.util.logger.LogHelper;
import org.apache.maven.artifact.versioning.ComparableVersion;

import javax.net.ssl.HttpsURLConnection;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;

public class Updater
{
    public UpdateStatus STATUS = UpdateStatus.NONE;
    protected String baseUrl = CoreReference.Updater.UPDATE_URL;
    protected String currentVersion;
    protected String minecraftVersion;
    protected String modId;
    protected boolean recommended = false;
    protected String downloadUrl;

    public Updater setCurrentVersion(String currentVersion) {
        this.currentVersion = currentVersion;

        return this;
    }

    public Updater setMinecraftVersion(String minecraftVersion) {
        this.minecraftVersion = minecraftVersion;

        return this;
    }

    public Updater setRecommendedOnly(boolean latest) {
        this.recommended = latest;

        return this;
    }

    public Updater setModId(String modId) {
        this.modId = modId;

        return this;
    }

    public void checkForUpdate() throws IOException {
        String version = this.recommended ? getRecommendedVersion() : getLatestVersion();

        if(version == null) {
            STATUS = UpdateStatus.UNSUPPORTED;
            return;
        }

        beginUpdateCheck();
    }

    public void beginUpdateCheck() {
        String versionData = null;

        try {
            STATUS = UpdateStatus.PENDING;
            versionData = getVersionsData();

            if(versionData == null) {
                STATUS = UpdateStatus.UNKNOWN;
                return;
            }

        } catch (IOException ignored) {}

        if(versionData != null) {
            String version = this.recommended ? getRecommendedVersion(versionData) : getLatestVersion(versionData);

            if(version != null) {
                processVersionStatus(version);
                if(this.getStatus() == UpdateStatus.OUTDATED) {
                    setDownloadUrl(getDownloadUrl(versionData, this.recommended ? CoreReference.Strings.RECOMMENDED.toLowerCase() : CoreReference.Strings.LATEST.toLowerCase()));
                }
            } else {
                STATUS = UpdateStatus.UNKNOWN;
            }
        }
    }

    private String getVersionsData() throws IOException {
        String data = null;
        HttpsURLConnection connection = null;
        InputStream stream = null;
        BufferedReader bufferedReader = null;

        try {
            URL url = new URL(this.buildUpdateUrl());

            connection = (HttpsURLConnection) url.openConnection();

            connection.setRequestProperty("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36");

            connection.connect();

            stream = connection.getInputStream();

            bufferedReader = new BufferedReader(new InputStreamReader(stream));

            data = bufferedReader.readLine();
        } catch (IOException ignored) {
            LogHelper.error("Update checking encountered an error!");
        } finally {
            if(bufferedReader != null) {
                try {
                    bufferedReader.close();
                } catch (IOException exp) {
                    LogHelper.error("Failed to close buffer!");
                }
            }

            if(stream != null) {
                try {
                    stream.close();
                } catch (IOException exp) {
                    LogHelper.error("Failed to close input stream!");
                }
            }

            if(connection != null) {
                connection.disconnect();
            }
        }

        return data;
    }

    private String getRecommendedVersion(String versionsJson) {
        if(versionsJson == null) {
            return null;
        }

        try {
            Object obj = JsonParser.parseString(versionsJson);
            JsonObject jsonObject = (JsonObject) obj;

            JsonObject versions = jsonObject.getAsJsonObject("versions");
            JsonObject currentVersion = versions.getAsJsonObject(getMinecraftVersion());

            if(currentVersion == null) {
                return null;
            }

            return currentVersion.get(CoreReference.Strings.RECOMMENDED.toLowerCase()).getAsString();
        } catch (NullPointerException e) {
            LogHelper.error(e.getMessage());
        }

        return null;
    }

    private String getLatestVersion(String versionsJson) {
        if(versionsJson == null) {
            return null;
        }

        Object obj = JsonParser.parseString(versionsJson);
        JsonObject jsonObject = (JsonObject) obj;

        JsonObject versions = jsonObject.getAsJsonObject("versions");
        JsonObject currentVersion = versions.getAsJsonObject(getMinecraftVersion());

        if(currentVersion == null) {
            return null;
        }

        return currentVersion.get(CoreReference.Strings.LATEST.toLowerCase()).getAsString();
    }

    public String getRecommendedVersion() {
        try {
            return getRecommendedVersion(getVersionsData());
        } catch (IOException ignored) {
            return null;
        }
    }

    public String getLatestVersion() {
        try {
            return getLatestVersion(getVersionsData());
        } catch (IOException ignored) {}

        return null;
    }

    public String getVersionForOutput() {
        if(this.isRecommended()) {
            return this.getRecommendedVersion();
        }

        return this.getLatestVersion();
    }

    private void processVersionStatus(String versionToCompare) {
        ComparableVersion current = new ComparableVersion(this.getCurrentVersion());
        ComparableVersion recOrLatest = new ComparableVersion(versionToCompare);

        if(currentVersion.equals("0.0NONE") || currentVersion.equals("NONE")) {
            setStatus(UpdateStatus.NONE);
            LogHelper.info("Current version is NONE. Development environment?");
            return;
        }

        int versionDiff = recOrLatest.compareTo(current);

        if(versionDiff == 0) {
            setStatus(UpdateStatus.UP_TO_DATE);
        } else if(versionDiff < 0) {
            setStatus(UpdateStatus.AHEAD);
        } else {
            setStatus(UpdateStatus.OUTDATED);
        }
    }

    public UpdateStatus getStatus() {
        return this.STATUS;
    }

    public String getMinecraftVersion() {
        return this.minecraftVersion;
    }

    public String getModId() {
        return this.modId;
    }

    public String getBaseUrl() {
        return this.baseUrl;
    }

    public String buildUpdateUrl() {
        return this.getBaseUrl() + "/mc-mods/" + this.getModId() + "/versions";
    }

    public String getCurrentVersion() {
        return this.currentVersion;
    }

    public void setStatus(UpdateStatus status) {
        this.STATUS = status;
    }

    public String getDownloadUrl() {
        return this.downloadUrl;
    }

    private String getDownloadUrl(String versionsData, String downloadType) {
        if(versionsData == null) {
            return null;
        }

        Object obj = JsonParser.parseString(versionsData);
        JsonObject jsonObject = (JsonObject) obj;

        JsonObject versions = jsonObject.getAsJsonObject("versions");
        JsonObject currentVersion = versions.getAsJsonObject(getMinecraftVersion());

        if(currentVersion == null) {
            return null;
        }

        return currentVersion.get(CoreReference.Strings.DOWNLOAD.toLowerCase() + "-"  + downloadType).getAsString();
    }

    private void setDownloadUrl(String url) {
        this.downloadUrl = url;
    }

    public boolean isRecommended() {
        return this.recommended;
    }
}
    - name: package io.savagedev.savagecore.util.updater;

/*
 * UpdaterUtils.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import io.savagedev.savagecore.init.ModConfigs;
import io.savagedev.savagecore.util.reference.CoreReference;
import io.savagedev.savagecore.util.logger.LogHelper;
import net.minecraft.ChatFormatting;
import net.minecraft.Util;
import net.minecraft.network.chat.Component;
import net.minecraftforge.event.entity.player.PlayerEvent;

import java.io.IOException;

public class UpdaterUtils
{
    public static void sendUpdateMessageIfOutdated(String modName, PlayerEvent.PlayerLoggedInEvent event, Updater updater) {
        if(updater.getStatus() == UpdateStatus.OUTDATED) {
            String version = updater.getVersionForOutput();
            String outdatedText = ChatFormatting.AQUA + "[" + modName + "] " +
                    ChatFormatting.RESET +
                    "is " +
                    ChatFormatting.RED +
                    "outdated! " +
                    ChatFormatting.RESET +
                    "Newest Version: " +
                    ChatFormatting.GOLD + version +
                    ChatFormatting.RESET +
                    " Current Version: " +
                    ChatFormatting.RED +
                    updater.getCurrentVersion();
            String downloadText = CoreReference.Strings.DOWNLOAD;
            String update_url = updater.getDownloadUrl();

            event.getEntity().sendSystemMessage(Component.Serializer.fromJson("[{\"text\":\"" + outdatedText + "\"}," + "{\"text\":\" " + ChatFormatting.WHITE + "[" + ChatFormatting.GREEN + downloadText + ChatFormatting.WHITE + "]\"," + "\"color\":\"green\",\"hoverEvent\":{\"action\":\"show_text\",\"value\":" + "{\"text\":\"Click to download the latest version\",\"color\":\"yellow\"}}," + "\"clickEvent\":{\"action\":\"open_url\",\"value\":\"" + update_url + "\"}}]"));
        }
    }

    public static void initializeUpdateCheck(Updater updater) {
        if(ModConfigs.ENABLE_UPDATE_CHECK.get()) {
            LogHelper.info(CoreReference.Updater.BEGIN_UPDATE_CHECK, updater.getModId());
            try {
                LogHelper.info(CoreReference.Updater.UPDATE_CHECK);

                updater.checkForUpdate();

                if(updater.getStatus() == UpdateStatus.UNSUPPORTED) {
                    LogHelper.info(CoreReference.Updater.UNSUPPORTED);
                } else if(updater.getStatus() == UpdateStatus.UP_TO_DATE) {
                    LogHelper.info(CoreReference.Updater.UP_TO_DATE);
                } else if(updater.getStatus() == UpdateStatus.OUTDATED) {
                    LogHelper.info(CoreReference.Updater.OUTDATED, updater.getVersionForOutput());
                } else {
                    LogHelper.info(CoreReference.Updater.UPDATE_STATUS, updater.getStatus());
                }
            } catch (IOException e) {
                LogHelper.error(CoreReference.Updater.UPDATE_FAILED);

                e.printStackTrace();
            }
        } else {
            LogHelper.info(CoreReference.Updater.UPDATE_DISABLED);
        }
    }
}
      run: package io.savagedev.savagecore;

/*
 * SavageCore.java
 * Copyright (C) 2024 Savage - github.com/devsavage
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import io.savagedev.savagecore.init.ModConfigs;
import io.savagedev.savagecore.util.reference.CoreReference;
import net.minecraftforge.fml.ModLoadingContext;
import net.minecraftforge.fml.common.Mod;
import net.minecraftforge.fml.config.ModConfig;
import net.minecraftforge.fml.loading.FMLPaths;

import java.nio.file.Path;

import com.electronwill.nightconfig.core.file.CommentedFileConfig;
import com.electronwill.nightconfig.core.io.WritingMode;

@Mod(CoreReference.MOD_ID)
public class SavageCore
{
    public SavageCore() {
        ModLoadingContext.get().registerConfig(ModConfig.Type.COMMON, ModConfigs.COMMON);

        Path configPath = FMLPaths.CONFIGDIR.get().resolve("savagecore-common.toml");
        CommentedFileConfig configData = CommentedFileConfig.builder(configPath).sync().autosave().writingMode(WritingMode.REPLACE).build();

        configData.load();
        ModConfigs.COMMON.setConfig(configData);
    }
}
