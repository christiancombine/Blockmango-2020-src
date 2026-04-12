package net.minecraft.src;

import java.util.Random;

public class WorldGenMinable extends WorldGenerator
{
    /** The block ID of the ore to be placed using this generator. */
    private int minableBlockId;

    /** The number of blocks to generate. */
    private int numberOfBlocks;
    private int field_94523_c;

    public WorldGenMinable(int par1, int par2)
    {
        this(par1, par2, Block.stone.blockID);
    }

    public WorldGenMinable(int par1, int par2, int par3)
    {
        this.minableBlockId = par1;
        this.numberOfBlocks = par2;
        this.field_94523_c = par3;
    }

    public boolean generate(World world, Random random, int x, int y, int z)
    {
        float angle = random.nextFloat() * (float)Math.PI;
        double p1x = (double)((float)(x + 8) + MathHelper.sin(angle) * (float)this.numberOfBlocks / 8.0F);
        double p2x = (double)((float)(x + 8) - MathHelper.sin(angle) * (float)this.numberOfBlocks / 8.0F);
        double p1z = (double)((float)(z + 8) + MathHelper.cos(angle) * (float)this.numberOfBlocks / 8.0F);
        double p2z = (double)((float)(z + 8) - MathHelper.cos(angle) * (float)this.numberOfBlocks / 8.0F);
        double p1y = (double)(y + random.nextInt(3) - 2);
        double p2y = (double)(y + random.nextInt(3) - 2);

        for (int idx = 0; idx <= this.numberOfBlocks; ++idx)
        {
            double tx = p1x + (p2x - p1x) * (double)idx / (double)this.numberOfBlocks;
            double ty = p1y + (p2y - p1y) * (double)idx / (double)this.numberOfBlocks;
            double tz = p1z + (p2z - p1z) * (double)idx / (double)this.numberOfBlocks;
            double randRange = random.nextDouble() * (double)this.numberOfBlocks / 16.0D;
            double range = (double)(MathHelper.sin((float)idx * (float)Math.PI / (float)this.numberOfBlocks) + 1.0F) * randRange + 1.0D;
            // double range = (double)(MathHelper.sin((float)idx * (float)Math.PI / (float)this.numberOfBlocks) + 1.0F) * randRange + 1.0D;
            int minX = MathHelper.floor_double(tx - range / 2.0D);
            int minY = MathHelper.floor_double(ty - range / 2.0D);
            int minZ = MathHelper.floor_double(tz - range / 2.0D);
            int maxX = MathHelper.floor_double(tx + range / 2.0D);
            int maxY = MathHelper.floor_double(ty + range / 2.0D);
            int maxZ = MathHelper.floor_double(tz + range / 2.0D);

            for (int ix = minX; ix <= maxX; ++ix)
            {
                double ratioX = ((double)ix + 0.5D - tx) / (range / 2.0D);

                if (ratioX * ratioX >= 1.0D)
                continue;

                for (int iy = minY; iy <= maxY; ++iy)
                {
                    double ratioY = ((double)iy + 0.5D - ty) / (range / 2.0D);

                    if (ratioX * ratioX + ratioY * ratioY >= 1.0D)
                        continue;

                    for (int iz = minZ; iz <= maxZ; ++iz)
                    {
                        double ratioZ = ((double)iz + 0.5D - tz) / (range / 2.0D);

                        if (ratioX * ratioX + ratioY * ratioY + ratioZ * ratioZ < 1.0D && world.getBlockId(ix, iy, iz) == this.field_94523_c)
                        {
                            world.setBlock(ix, iy, iz, this.minableBlockId, 0, 2);
                        }
                    }
                }
            }
        }

        return true;
    }
}
