namespace CSharpApp
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.btnAd1_1 = new System.Windows.Forms.Button();
            this.btnAd1_2 = new System.Windows.Forms.Button();
            this.btnAd5 = new System.Windows.Forms.Button();
            this.btnAd6 = new System.Windows.Forms.Button();
            this.btnAd4 = new System.Windows.Forms.Button();
            this.btnAd3 = new System.Windows.Forms.Button();
            this.btnAd2 = new System.Windows.Forms.Button();
            this.btnAd1 = new System.Windows.Forms.Button();
            this.panelAd = new System.Windows.Forms.Panel();
            this.textBox1 = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.btnAd1_1);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd1_2);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd5);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd6);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd4);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd3);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd2);
            this.splitContainer1.Panel1.Controls.Add(this.btnAd1);
            this.splitContainer1.Panel1.Controls.Add(this.panelAd);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.textBox1);
            this.splitContainer1.Size = new System.Drawing.Size(1264, 791);
            this.splitContainer1.SplitterDistance = 420;
            this.splitContainer1.TabIndex = 1;
            // 
            // btnAd1_1
            // 
            this.btnAd1_1.Location = new System.Drawing.Point(139, 220);
            this.btnAd1_1.Name = "btnAd1_1";
            this.btnAd1_1.Size = new System.Drawing.Size(132, 23);
            this.btnAd1_1.TabIndex = 13;
            this.btnAd1_1.Text = "插屏广告，指定控件";
            this.btnAd1_1.UseVisualStyleBackColor = true;
            this.btnAd1_1.Click += new System.EventHandler(this.btnAd1_1_Click);
            // 
            // btnAd1_2
            // 
            this.btnAd1_2.Location = new System.Drawing.Point(139, 9);
            this.btnAd1_2.Name = "btnAd1_2";
            this.btnAd1_2.Size = new System.Drawing.Size(132, 23);
            this.btnAd1_2.TabIndex = 12;
            this.btnAd1_2.Text = "插屏广告，仅视频";
            this.btnAd1_2.UseVisualStyleBackColor = true;
            this.btnAd1_2.Click += new System.EventHandler(this.btnAd1_2_Click);
            // 
            // btnAd5
            // 
            this.btnAd5.Location = new System.Drawing.Point(14, 174);
            this.btnAd5.Name = "btnAd5";
            this.btnAd5.Size = new System.Drawing.Size(88, 23);
            this.btnAd5.TabIndex = 11;
            this.btnAd5.Text = "5.激励视频";
            this.btnAd5.UseVisualStyleBackColor = true;
            this.btnAd5.Click += new System.EventHandler(this.btnAd5_Click);
            // 
            // btnAd6
            // 
            this.btnAd6.Location = new System.Drawing.Point(313, 9);
            this.btnAd6.Name = "btnAd6";
            this.btnAd6.Size = new System.Drawing.Size(88, 23);
            this.btnAd6.TabIndex = 10;
            this.btnAd6.Text = "6.删除广告";
            this.btnAd6.UseVisualStyleBackColor = true;
            this.btnAd6.Click += new System.EventHandler(this.btnAd6_Click);
            // 
            // btnAd4
            // 
            this.btnAd4.Location = new System.Drawing.Point(14, 133);
            this.btnAd4.Name = "btnAd4";
            this.btnAd4.Size = new System.Drawing.Size(88, 23);
            this.btnAd4.TabIndex = 9;
            this.btnAd4.Text = "4.对联广告";
            this.btnAd4.UseVisualStyleBackColor = true;
            this.btnAd4.Click += new System.EventHandler(this.btnAd4_Click);
            // 
            // btnAd3
            // 
            this.btnAd3.Location = new System.Drawing.Point(14, 89);
            this.btnAd3.Name = "btnAd3";
            this.btnAd3.Size = new System.Drawing.Size(88, 23);
            this.btnAd3.TabIndex = 8;
            this.btnAd3.Text = "3.横幅广告";
            this.btnAd3.UseVisualStyleBackColor = true;
            this.btnAd3.Click += new System.EventHandler(this.btnAd3_Click);
            // 
            // btnAd2
            // 
            this.btnAd2.Location = new System.Drawing.Point(14, 49);
            this.btnAd2.Name = "btnAd2";
            this.btnAd2.Size = new System.Drawing.Size(88, 23);
            this.btnAd2.TabIndex = 7;
            this.btnAd2.Text = "2.全屏插播";
            this.btnAd2.UseVisualStyleBackColor = true;
            this.btnAd2.Click += new System.EventHandler(this.btnAd2_Click);
            // 
            // btnAd1
            // 
            this.btnAd1.Location = new System.Drawing.Point(14, 9);
            this.btnAd1.Name = "btnAd1";
            this.btnAd1.Size = new System.Drawing.Size(88, 23);
            this.btnAd1.TabIndex = 6;
            this.btnAd1.Text = "1.插屏广告";
            this.btnAd1.UseVisualStyleBackColor = true;
            this.btnAd1.Click += new System.EventHandler(this.btnAd1_Click);
            // 
            // panelAd
            // 
            this.panelAd.Location = new System.Drawing.Point(12, 249);
            this.panelAd.Name = "panelAd";
            this.panelAd.Size = new System.Drawing.Size(400, 400);
            this.panelAd.TabIndex = 3;
            // 
            // textBox1
            // 
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Location = new System.Drawing.Point(0, 0);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(840, 791);
            this.textBox1.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1264, 791);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button btnAd5;
        private System.Windows.Forms.Button btnAd6;
        private System.Windows.Forms.Button btnAd4;
        private System.Windows.Forms.Button btnAd3;
        private System.Windows.Forms.Button btnAd2;
        private System.Windows.Forms.Button btnAd1;
        private System.Windows.Forms.Panel panelAd;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnAd1_2;
        private System.Windows.Forms.Button btnAd1_1;
    }
}

