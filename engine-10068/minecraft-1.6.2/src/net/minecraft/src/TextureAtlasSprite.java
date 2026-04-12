package net.minecraft.src;

import com.google.common.collect.Lists;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import javax.imageio.ImageIO;

public class TextureAtlasSprite implements Icon
{
    private final String field_110984_i;
    protected List field_110976_a = Lists.newArrayList();
    private AnimationMetadataSection field_110982_k;
    protected boolean field_130222_e;
    protected int field_110975_c;
    protected int field_110974_d;
    protected int field_130223_c;
    protected int field_130224_d;
    private float field_110979_l;
    private float field_110980_m;
    private float field_110977_n;
    private float field_110978_o;
    protected int field_110973_g;
    protected int field_110983_h;

    protected TextureAtlasSprite(String par1Str)
    {
        this.field_110984_i = par1Str;
    }

    public void func_110971_a(int par1, int par2, int par3, int par4, boolean par5)
    {
        this.field_110975_c = par3;
        this.field_110974_d = par4;
        this.field_130222_e = par5;
        float var6 = (float)(0.009999999776482582D / (double)par1);
        float var7 = (float)(0.009999999776482582D / (double)par2);
        this.field_110979_l = (float)par3 / (float)((double)par1) + var6;
        this.field_110980_m = (float)(par3 + this.field_130223_c) / (float)((double)par1) - var6;
        this.field_110977_n = (float)par4 / (float)par2 + var7;
        this.field_110978_o = (float)(par4 + this.field_130224_d) / (float)par2 - var7;
    }

    public void copyFrom(TextureAtlasSprite par1TextureAtlasSprite)
    {
        this.field_110975_c = par1TextureAtlasSprite.field_110975_c;
        this.field_110974_d = par1TextureAtlasSprite.field_110974_d;
        this.field_130223_c = par1TextureAtlasSprite.field_130223_c;
        this.field_130224_d = par1TextureAtlasSprite.field_130224_d;
        this.field_130222_e = par1TextureAtlasSprite.field_130222_e;
        this.field_110979_l = par1TextureAtlasSprite.field_110979_l;
        this.field_110980_m = par1TextureAtlasSprite.field_110980_m;
        this.field_110977_n = par1TextureAtlasSprite.field_110977_n;
        this.field_110978_o = par1TextureAtlasSprite.field_110978_o;
    }

    public int func_130010_a()
    {
        return this.field_110975_c;
    }

    public int func_110967_i()
    {
        return this.field_110974_d;
    }

    /**
     * Returns the X position of this icon on its texture sheet, in pixels.
     */
    public int getOriginX()
    {
        return this.field_130223_c;
    }

    /**
     * Returns the Y position of this icon on its texture sheet, in pixels.
     */
    public int getOriginY()
    {
        return this.field_130224_d;
    }

    /**
     * Returns the minimum U coordinate to use when rendering with this icon.
     */
    public float getMinU()
    {
        return this.field_110979_l;
    }

    /**
     * Returns the maximum U coordinate to use when rendering with this icon.
     */
    public float getMaxU()
    {
        return this.field_110980_m;
    }

    /**
     * Gets a U coordinate on the icon. 0 returns uMin and 16 returns uMax. Other arguments return in-between values.
     */
    public float getInterpolatedU(double par1)
    {
        float var3 = this.field_110980_m - this.field_110979_l;
        return this.field_110979_l + var3 * (float)par1 / 16.0F;
    }

    /**
     * Returns the minimum V coordinate to use when rendering with this icon.
     */
    public float getMinV()
    {
        return this.field_110977_n;
    }

    /**
     * Returns the maximum V coordinate to use when rendering with this icon.
     */
    public float getMaxV()
    {
        return this.field_110978_o;
    }

    /**
     * Gets a V coordinate on the icon. 0 returns vMin and 16 returns vMax. Other arguments return in-between values.
     */
    public float getInterpolatedV(double par1)
    {
        float var3 = this.field_110978_o - this.field_110977_n;
        return this.field_110977_n + var3 * ((float)par1 / 16.0F);
    }

    public String getIconName()
    {
        return this.field_110984_i;
    }

    public void updateAnimation()
    {
        ++this.field_110983_h;

        if (this.field_110983_h >= this.field_110982_k.func_110472_a(this.field_110973_g))
        {
            int var1 = this.field_110982_k.func_110468_c(this.field_110973_g);
            int var2 = this.field_110982_k.func_110473_c() == 0 ? this.field_110976_a.size() : this.field_110982_k.func_110473_c();
            this.field_110973_g = (this.field_110973_g + 1) % var2;
            this.field_110983_h = 0;
            int var3 = this.field_110982_k.func_110468_c(this.field_110973_g);

            if (var1 != var3 && var3 >= 0 && var3 < this.field_110976_a.size())
            {
                TextureUtil.func_110998_a((int[])this.field_110976_a.get(var3), this.field_130223_c, this.field_130224_d, this.field_110975_c, this.field_110974_d, false, false);
            }
        }
    }

    public int[] func_110965_a(int par1)
    {
        return (int[])this.field_110976_a.get(par1);
    }

    public int func_110970_k()
    {
        return this.field_110976_a.size();
    }

    public void func_110966_b(int par1)
    {
        this.field_130223_c = par1;
    }

    public void func_110969_c(int par1)
    {
        this.field_130224_d = par1;
    }

    public void func_130100_a(Resource par1Resource) throws IOException
    {
        this.func_130102_n();
        InputStream var2 = par1Resource.func_110527_b();
        AnimationMetadataSection var3 = (AnimationMetadataSection)par1Resource.func_110526_a("animation");
        BufferedImage var4 = ImageIO.read(var2);
        this.field_130224_d = var4.getHeight();
        this.field_130223_c = var4.getWidth();
        int[] var5 = new int[this.field_130224_d * this.field_130223_c];
        var4.getRGB(0, 0, this.field_130223_c, this.field_130224_d, var5, 0, this.field_130223_c);

        if (var3 == null)
        {
            if (this.field_130224_d != this.field_130223_c)
            {
                throw new RuntimeException("broken aspect ratio and not an animation");
            }

            this.field_110976_a.add(var5);
        }
        else
        {
            int var6 = this.field_130224_d / this.field_130223_c;
            int var7 = this.field_130223_c;
            int var8 = this.field_130223_c;
            this.field_130224_d = this.field_130223_c;
            int var10;

            if (var3.func_110473_c() > 0)
            {
                Iterator var9 = var3.func_130073_e().iterator();

                while (var9.hasNext())
                {
                    var10 = ((Integer)var9.next()).intValue();

                    if (var10 >= var6)
                    {
                        throw new RuntimeException("invalid frameindex " + var10);
                    }

                    this.func_130099_d(var10);
                    this.field_110976_a.set(var10, func_130101_a(var5, var7, var8, var10));
                }

                this.field_110982_k = var3;
            }
            else
            {
                ArrayList var11 = Lists.newArrayList();

                for (var10 = 0; var10 < var6; ++var10)
                {
                    this.field_110976_a.add(func_130101_a(var5, var7, var8, var10));
                    var11.add(new AnimationFrame(var10, -1));
                }

                this.field_110982_k = new AnimationMetadataSection(var11, this.field_130223_c, this.field_130224_d, var3.func_110469_d());
            }
        }
    }

    private void func_130099_d(int par1)
    {
        if (this.field_110976_a.size() <= par1)
        {
            for (int var2 = this.field_110976_a.size(); var2 <= par1; ++var2)
            {
                this.field_110976_a.add((Object)null);
            }
        }
    }

    private static int[] func_130101_a(int[] par0ArrayOfInteger, int par1, int par2, int par3)
    {
        int[] var4 = new int[par1 * par2];
        System.arraycopy(par0ArrayOfInteger, par3 * var4.length, var4, 0, var4.length);
        return var4;
    }

    public void func_130103_l()
    {
        this.field_110976_a.clear();
    }

    public boolean func_130098_m()
    {
        return this.field_110982_k != null;
    }

    public void func_110968_a(List par1List)
    {
        this.field_110976_a = par1List;
    }

    private void func_130102_n()
    {
        this.field_110982_k = null;
        this.func_110968_a(Lists.newArrayList());
        this.field_110973_g = 0;
        this.field_110983_h = 0;
    }

    public String toString()
    {
        return "TextureAtlasSprite{name=\'" + this.field_110984_i + '\'' + ", frameCount=" + this.field_110976_a.size() + ", rotated=" + this.field_130222_e + ", x=" + this.field_110975_c + ", y=" + this.field_110974_d + ", height=" + this.field_130224_d + ", width=" + this.field_130223_c + ", u0=" + this.field_110979_l + ", u1=" + this.field_110980_m + ", v0=" + this.field_110977_n + ", v1=" + this.field_110978_o + '}';
    }
}
