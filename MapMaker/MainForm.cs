using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Text;

namespace MapMaker
{
    public partial class MainForm : Form
    {
        bool tileMode = true;
        List<Tile> tiles = new List<Tile>();
        List<Tile> tilesSprites = new List<Tile>();
        List<TilePiece> tilePieces = new List<TilePiece>();
        FontFamily arialFont;
        Font tileTextFont;
        Rectangle screenRect;
        int selectedTile = 0;
        int mouseX = 0;
        int mouseY = 0;
        int outputWidth = 128;
        int outputHeight = 128;
        int tileSize = 16;
        bool placingTiles = false;

        public MainForm()
        {
            InitializeComponent();

            BackColor = Color.FromArgb(34, 34, 34);

            List<string> TileFiles = new List<string>();
            TileFiles.AddRange(Directory.GetFiles(@"C:\Development\GingerGinny\Images\DungeonTilesetII_v1.4", "floor*.png"));
            TileFiles.AddRange(Directory.GetFiles(@"C:\Development\GingerGinny\Images\DungeonTilesetII_v1.4", "wall*.png"));
            TileFiles.Sort();

            List<string> SpriteFiles = new List<string>();
            SpriteFiles.AddRange(Directory.GetFiles(@"C:\Development\GingerGinny\Images\DungeonTilesetII_v1.4", "*.png"));
            SpriteFiles.RemoveAll(x => TileFiles.Contains(x));

            foreach (var tileFile in TileFiles)
                tiles.Add(new Tile(tileFile));

            foreach (var tileFile in SpriteFiles)
                tilesSprites.Add(new Tile(tileFile, true));

            hScrollBar.Maximum = tiles.Count;

            EnableDoubleBuffering();

            arialFont = new FontFamily("Arial");
            tileTextFont = new Font(arialFont, 14, FontStyle.Regular, GraphicsUnit.Pixel);

            MouseWheel += MainForm_MouseWheel;

            screenRect = new Rectangle(0, menuStrip1.Location.Y + menuStrip1.Height, vScrollBarView.Location.X, hScrollBarView.Location.Y - (menuStrip1.Location.Y + menuStrip1.Height));
        }

        private void MainForm_MouseWheel(object sender, MouseEventArgs e)
        {
            // Over actual tile
            int mouseOverTileX = (mouseX / tileSize) + hScrollBarView.Value;
            int mouseOverTileY = (mouseY / tileSize) + vScrollBarView.Value;

            if (e.Delta > 0)
            {
                if (tileSize <= 128)
                {
                    tileSize *= 2;

                    int maxTilesX = screenRect.Width / tileSize;
                    int maxTilesY = screenRect.Height / tileSize;

                    hScrollBarView.Value = Math.Max(mouseOverTileX - (maxTilesX / 2), 0);
                    vScrollBarView.Value = Math.Max(mouseOverTileY - (maxTilesY / 2), 0);
                }
            }
            if (e.Delta < 0)
            {
                if (tileSize > 16)
                {
                    tileSize /= 2;

                    int maxTilesX = screenRect.Width / tileSize;
                    int maxTilesY = screenRect.Height / tileSize;

                    hScrollBarView.Value = Math.Max(mouseOverTileX - (maxTilesX / 2), 0);
                    vScrollBarView.Value = Math.Max(mouseOverTileY - (maxTilesY / 2), 0);
                }
            }
            Invalidate();
        }

        public void EnableDoubleBuffering()
        {
            this.SetStyle(ControlStyles.DoubleBuffer |
               ControlStyles.UserPaint |
               ControlStyles.AllPaintingInWmPaint,
               true);
            this.UpdateStyles();
        }

        private void MainForm_Paint(object sender, PaintEventArgs e)
        {
            var g = e.Graphics;

            // Set so it gets that pixel font look
            g.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.Half;
            g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.AssumeLinear;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;

            var selectedTiles = GetSelectedTiles();

            //g.DrawRectangle(Pens.Red, screenRect);

            // Draw tile selection at bottom
            for (int i = hScrollBar.Value; i < hScrollBar.Value + 10; i++)
            {
                if (i < selectedTiles.Count)
                {
                    int x = ((5 + outputWidth) * (i - hScrollBar.Value));
                    g.DrawImage(selectedTiles[i].image, x, hScrollBar.Location.Y + 35, outputWidth, outputHeight);
                    var rect = new Rectangle(x, hScrollBar.Location.Y + 35 + 5 + outputHeight, outputWidth, 60);
                    g.DrawString(string.Format("{0}. {1}", i, selectedTiles[i].shortName), tileTextFont, Brushes.White, rect);
                }
            }

            // Draw the currently placed tiles
            foreach (var tile in tilePieces)
            {
                if (((tile.y - vScrollBarView.Value) * tileSize) + tileSize < hScrollBar.Location.Y)
                {
                    g.DrawImage(tile.facingRight ? tile.tile.image : tile.tile.flippedImage, (tile.x - hScrollBarView.Value) * tileSize, (tile.y - vScrollBarView.Value) * tileSize, tileSize, tileSize);
                }
            }

            // Draw the tile on the mouse pointer and list the current mouse location
            if (mouseY < hScrollBar.Location.Y)
            {
                if (selectedTile != -1)
                {
                    int tileX = mouseX;
                    int tileY = mouseY;
                    tileX = ((tileX / tileSize) * tileSize);
                    tileY = ((tileY / tileSize) * tileSize);
                    g.DrawImage(selectedTiles[selectedTile].image, tileX, tileY, tileSize, tileSize);
                }

                int mouseOverTileX = (mouseX / tileSize) + hScrollBarView.Value;
                int mouseOverTileY = (mouseY / tileSize) + vScrollBarView.Value;
                g.DrawString(string.Format("X: {0} Y: {1} Zoom: {2}x{2} Scroll: {3}x{4}", mouseOverTileX, mouseOverTileY, tileSize, hScrollBarView.Value, vScrollBarView.Value), tileTextFont, Brushes.White, 0, hScrollBarView.Location.Y - 20);
            }
        }

        private void hScrollBar_ValueChanged(object sender, EventArgs e)
        {
            Invalidate();
        }

        private void MainForm_MouseMove(object sender, MouseEventArgs e)
        {
            mouseX = e.X;
            mouseY = e.Y;
            PlaceTile();
            Invalidate();
        }

        List<Tile> GetSelectedTiles()
        {
            return tileMode ? tiles : tilesSprites;
        }

        private void MainForm_MouseClick(object sender, MouseEventArgs e)
        {
            var selectedTiles = GetSelectedTiles();

            if (mouseY >= hScrollBar.Location.Y)
            {
                for (int i = hScrollBar.Value; i < hScrollBar.Value + 10; i++)
                {
                    if (i < selectedTiles.Count)
                    {
                        int x = ((5 + outputWidth) * (i - hScrollBar.Value));
                        var rect = new Rectangle(x, hScrollBar.Location.Y + 35, outputWidth, outputHeight);
                        if (rect.Contains(mouseX, mouseY))
                        {
                            selectedTile = i;
                            break;
                        }
                    }
                }
            }

            if (selectedTile != -1 && mouseY < hScrollBar.Location.Y)
            {
                mouseX = e.X;
                mouseY = e.Y;

                if (e.Button == MouseButtons.Right)
                {
                    // Remove the last placed one
                    for (int i = tilePieces.Count - 1; i >= 0; i--)
                    {
                        if (tilePieces[i].x == (e.X / tileSize) + hScrollBarView.Value && tilePieces[i].y == (e.Y / tileSize) + vScrollBarView.Value)
                        {
                            tilePieces.RemoveAt(i);
                            break;
                        }
                    }
                }

                Invalidate();
            }
        }

        private void MainForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (selectedTile != -1 && mouseY < hScrollBar.Location.Y)
                {
                    mouseX = e.X;
                    mouseY = e.Y;

                    placingTiles = true;
                    PlaceTile();
                }
            }
        }

        private void MainForm_MouseUp(object sender, MouseEventArgs e)
        {
            placingTiles = false;
        }

        private void PlaceTile()
        {
            if (placingTiles)
            {
                var selectedTiles = GetSelectedTiles();
                var tilePiece = new TilePiece();
                tilePiece.x = (mouseX / tileSize) + hScrollBarView.Value;
                tilePiece.y = (mouseY / tileSize) + vScrollBarView.Value;
                tilePiece.tile = selectedTiles[selectedTile];

                if (tileMode)
                {
                    if (tilePieces.FirstOrDefault(x => x.x == tilePiece.x && x.y == tilePiece.y && x.tile == tilePiece.tile) == null)
                        tilePieces.Add(tilePiece);
                }
                else
                {
                    // Sprite mode - so flip the existing
                    var existingSprite = tilePieces.FirstOrDefault(x => x.x == tilePiece.x && x.y == tilePiece.y && x.tile == tilePiece.tile);
                    if (existingSprite == null)
                        tilePieces.Add(tilePiece);
                    else
                        existingSprite.facingRight = !existingSprite.facingRight;
                }
                
            }

            Invalidate();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.DefaultExt = "txt";
            sfd.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                using (var sw = File.CreateText(sfd.FileName))
                { 
                    foreach (var tilePiece in tilePieces)
                    {
                        sw.WriteLine(string.Format("{0} {1} {2} {3} {4}", tilePiece.x, tilePiece.y, tilePiece.tile.shortName, tilePiece.tile.isSprite ? "S" : "T", tilePiece.facingRight ? "R" : "L"));
                    }
                }
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var ofd = new OpenFileDialog();
            ofd.DefaultExt = "txt";
            ofd.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                tilePieces.Clear();
                var lines = File.ReadAllLines(ofd.FileName);
                foreach (var line in lines)
                {
                    var split = line.Split(' ');
                    if (split.Length == 5)
                    {
                        var newTilePiece = new TilePiece();
                        newTilePiece.x = int.Parse(split[0]);
                        newTilePiece.y = int.Parse(split[1]);
                        var tile = tiles.FirstOrDefault(x => x.shortName == split[2]);
                        if (tile == null)
                            tile = tilesSprites.First(x => x.shortName == split[2]);
                        newTilePiece.tile = tile;
                        newTilePiece.facingRight = (split[4] == "R");
                        tilePieces.Add(newTilePiece);
                    }
                }
                Invalidate();
            }
        }

        private void hScrollBarView_ValueChanged(object sender, EventArgs e)
        {
            Invalidate();
        }

        private void vScrollBarView_ValueChanged(object sender, EventArgs e)
        {
            Invalidate();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            tilePieces.Clear();
            Invalidate();
        }

        private void buttonMode_Click(object sender, EventArgs e)
        {
            tileMode = !tileMode;

            hScrollBar.Maximum = GetSelectedTiles().Count;

            Invalidate();
        }
    }

    public class TilePiece
    {
        public Tile tile;
        public int x;
        public int y;
        public bool facingRight = true;
    }

    public class Tile
    {
        public Tile(string fileName, bool isSprite = false)
        {
            image = new Bitmap(fileName);
            flippedImage = new Bitmap(image);
            flippedImage.RotateFlip(RotateFlipType.RotateNoneFlipX);
            shortName = Path.GetFileName(fileName);
            this.fileName = fileName;
            this.isSprite = isSprite;
        }
        public string fileName;
        public string shortName;
        public Bitmap image;
        public Bitmap flippedImage;
        public bool isSprite;
    }
}
