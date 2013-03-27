using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace IdleVisualizer
{
    public partial class Form1 : Form
    {
        private List<byte> perc;
        private List<bool> mouseMovement;
        private Thread refreshThread;

        public Form1()
        {
            InitializeComponent();

            perc = new List<byte>();
            mouseMovement = new List<bool>();

            refreshThread = new Thread(new ThreadStart(() =>
            {
                while (true)
                {
                    this.Invoke(new Action(() => this.Refresh()));
                    Thread.Sleep(2000);
                }
            }));
            refreshThread.Start();
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            LoadData();

            int x = 0;
            int y;
            int step = (int)Math.Ceiling(perc.Count / (float)Width);

            for (int i = 0; i < perc.Count; i+=step )
            {
                x++;
                y = this.Height - (perc[i]);
                e.Graphics.FillRectangle(mouseMovement[i] ? Brushes.Red : Brushes.Black, x, y, 1, y);
            }
        }
        

        private void LoadData()
        {
            try
            {
                BinaryReader db = new BinaryReader(File.Open("C:\\idle.db", FileMode.Open, FileAccess.Read, FileShare.ReadWrite));
                int cpuNumber = db.ReadInt32();

                perc.Clear();
                mouseMovement.Clear();

                while (db.BaseStream.Position < db.BaseStream.Length - 1)
                {
                    mouseMovement.Add(db.ReadBoolean());

                    for (int i = 0; i < cpuNumber; i++)
                    {
                        byte idlePerc = db.ReadByte();

                        if (i == 0)
                            perc.Add(idlePerc);
                    }
                }
            }
            catch (Exception e) { }
            //this.Refresh();
            //Application.DoEvents();
            //System.Threading.Thread.Sleep(2000);
        }
    }
}
