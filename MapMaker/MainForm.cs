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
        List<Tile> tiles = new List<Tile>();
        List<TilePiece> tilePieces = new List<TilePiece>();
        FontFamily arialFont;
        Font tileTextFont;
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

            foreach (var tileFile in TileFiles)
            {
                tiles.Add(new Tile(tileFile));
            }

            hScrollBar.Maximum = tiles.Count;

            EnableDoubleBuffering();

            arialFont = new FontFamily("Arial");
            tileTextFont = new Font(arialFont, 14, FontStyle.Regular, GraphicsUnit.Pixel);

            MouseWheel += MainForm_MouseWheel;
        }

        private void MainForm_MouseWheel(object sender, MouseEventArgs e)
        {
            if (e.Delta > 0)
            {
                if (tileSize <= 128)
                    tileSize *= 2;
            }
            else
            {
                if (tileSize > 16)
                    tileSize /= 2;
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


            // Draw tile selection at bottom
            for (int i = hScrollBar.Value; i < hScrollBar.Value + 10; i++)
            {
                if (i < tiles.Count)
                {
                    int x = ((5 + outputWidth) * (i - hScrollBar.Value));
                    g.DrawImage(tiles[i].image, x, hScrollBar.Location.Y + 35, outputWidth, outputHeight);
                    var rect = new Rectangle(x, hScrollBar.Location.Y + 35 + 5 + outputHeight, outputWidth, 60);
                    g.DrawString(string.Format("{0}. {1}", i, tiles[i].shortName), tileTextFont, Brushes.White, rect);
                }
            }

            // Draw the currently placed tiles
            foreach (var tile in tilePieces)
            {
                if (((tile.y - vScrollBarView.Value) * tileSize) + tileSize < hScrollBar.Location.Y)
                    g.DrawImage(tile.tile.image, (tile.x - hScrollBarView.Value) * tileSize, (tile.y - vScrollBarView.Value) * tileSize, tileSize, tileSize);
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
                    g.DrawImage(tiles[selectedTile].image, tileX, tileY, tileSize, tileSize);
                }
                g.DrawString(string.Format("X: {0} Y: {1} Zoom: {2}x{2}", mouseX / tileSize, mouseY / tileSize, tileSize), tileTextFont, Brushes.White, 0, hScrollBarView.Location.Y - 20);
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

        private void MainForm_MouseClick(object sender, MouseEventArgs e)
        {
            if (mouseY >= hScrollBar.Location.Y)
            {
                for (int i = hScrollBar.Value; i < hScrollBar.Value + 10; i++)
                {
                    if (i < tiles.Count)
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
                var tilePiece = new TilePiece();
                tilePiece.x = (mouseX / tileSize) + hScrollBarView.Value;
                tilePiece.y = (mouseY / tileSize) + vScrollBarView.Value;
                tilePiece.tile = tiles[selectedTile];

                if (tilePieces.FirstOrDefault(x => x.x == tilePiece.x && x.y == tilePiece.y && x.tile == tilePiece.tile) == null)
                    tilePieces.Add(tilePiece);
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
                        sw.WriteLine(string.Format("{0} {1} {2}", tilePiece.x, tilePiece.y, tilePiece.tile.shortName));
                    }
                }
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.DefaultExt = "txt";
            ofd.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                tilePieces.Clear();
                var lines = File.ReadAllLines(ofd.FileName);
                foreach (var line in lines)
                {
                    var split = line.Split(' ');
                    if (split.Length == 3)
                    {
                        var newTilePiece = new TilePiece();
                        newTilePiece.x = int.Parse(split[0]);
                        newTilePiece.y = int.Parse(split[1]);
                        newTilePiece.tile = tiles.First(x => x.shortName == split[2]);
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
    }

    public class TilePiece
    {
        public Tile tile;
        public int x;
        public int y;
    }

    public class Tile
    {
        public Tile(string fileName)
        {
            image = new Bitmap(fileName);
            shortName = Path.GetFileName(fileName);
            this.fileName = fileName;
        }
        public string fileName;
        public string shortName;
        public Bitmap image;
    }
}
