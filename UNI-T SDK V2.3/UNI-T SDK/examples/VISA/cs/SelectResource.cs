using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using NationalInstruments.VisaNS;

namespace NationalInstruments.Examples.SimpleReadWrite
{
    public class SelectResource : System.Windows.Forms.Form
    {
        private System.Windows.Forms.ListBox availableResourcesListBox;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.TextBox visaResourceNameTextBox;
        private System.Windows.Forms.Label AvailableResourcesLabel;
        private System.Windows.Forms.Label ResourceStringLabel;


        private System.ComponentModel.Container components = null;

        public SelectResource()
        {

            InitializeComponent();
        }

        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        private void InitializeComponent()
        {
            this.availableResourcesListBox = new System.Windows.Forms.ListBox();
            this.okButton = new System.Windows.Forms.Button();
            this.closeButton = new System.Windows.Forms.Button();
            this.visaResourceNameTextBox = new System.Windows.Forms.TextBox();
            this.AvailableResourcesLabel = new System.Windows.Forms.Label();
            this.ResourceStringLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // availableResourcesListBox
            // 
            this.availableResourcesListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.availableResourcesListBox.ItemHeight = 12;
            this.availableResourcesListBox.Location = new System.Drawing.Point(6, 19);
            this.availableResourcesListBox.Name = "availableResourcesListBox";
            this.availableResourcesListBox.Size = new System.Drawing.Size(280, 88);
            this.availableResourcesListBox.TabIndex = 0;
            this.availableResourcesListBox.SelectedIndexChanged += new System.EventHandler(this.availableResourcesListBox_SelectedIndexChanged);
            this.availableResourcesListBox.DoubleClick += new System.EventHandler(this.availableResourcesListBox_DoubleClick);
            // 
            // okButton
            // 
            this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(6, 191);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(92, 27);
            this.okButton.TabIndex = 2;
            this.okButton.Text = "OK";
            // 
            // closeButton
            // 
            this.closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.closeButton.Location = new System.Drawing.Point(98, 191);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(93, 27);
            this.closeButton.TabIndex = 3;
            this.closeButton.Text = "Cancel";
            // 
            // visaResourceNameTextBox
            // 
            this.visaResourceNameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.visaResourceNameTextBox.Location = new System.Drawing.Point(6, 159);
            this.visaResourceNameTextBox.Name = "visaResourceNameTextBox";
            this.visaResourceNameTextBox.Size = new System.Drawing.Size(280, 21);
            this.visaResourceNameTextBox.TabIndex = 4;
            this.visaResourceNameTextBox.Text = "GPIB0::2::INSTR";
            // 
            // AvailableResourcesLabel
            // 
            this.AvailableResourcesLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.AvailableResourcesLabel.Location = new System.Drawing.Point(6, 5);
            this.AvailableResourcesLabel.Name = "AvailableResourcesLabel";
            this.AvailableResourcesLabel.Size = new System.Drawing.Size(277, 13);
            this.AvailableResourcesLabel.TabIndex = 5;
            this.AvailableResourcesLabel.Text = "Available Resources:";
            // 
            // ResourceStringLabel
            // 
            this.ResourceStringLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ResourceStringLabel.Location = new System.Drawing.Point(6, 142);
            this.ResourceStringLabel.Name = "ResourceStringLabel";
            this.ResourceStringLabel.Size = new System.Drawing.Size(277, 14);
            this.ResourceStringLabel.TabIndex = 6;
            this.ResourceStringLabel.Text = "Resource String:";
            // 
            // SelectResource
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.CancelButton = this.closeButton;
            this.ClientSize = new System.Drawing.Size(292, 227);
            this.Controls.Add(this.ResourceStringLabel);
            this.Controls.Add(this.AvailableResourcesLabel);
            this.Controls.Add(this.visaResourceNameTextBox);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.availableResourcesListBox);
            this.KeyPreview = true;
            this.MaximizeBox = false;
            this.MinimumSize = new System.Drawing.Size(212, 266);
            this.Name = "SelectResource";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Select Resource";
            this.Load += new System.EventHandler(this.OnLoad);
            this.ResumeLayout(false);
            this.PerformLayout();

        }


        private void OnLoad(object sender, System.EventArgs e)
        {
            string[] resources = ResourceManager.GetLocalManager().FindResources("?*");
            foreach(string s in resources)
            {
                availableResourcesListBox.Items.Add(s);
            }
            availableResourcesListBox.Items.Add("TCPIP[board]::host address[::LAN device name][::INSTR]");
            availableResourcesListBox.Items.Add("TCPIP[board]::host address::port::SOCKET");
        }

        private void availableResourcesListBox_DoubleClick(object sender, System.EventArgs e)
        {
            string selectedString = (string)availableResourcesListBox.SelectedItem;
            ResourceName = selectedString;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void availableResourcesListBox_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            string selectedString = (string)availableResourcesListBox.SelectedItem;
            ResourceName = selectedString;
        }

        public string ResourceName
        {
            get
            {
                return visaResourceNameTextBox.Text;
            }
            set
            {
                visaResourceNameTextBox.Text = value;
            }
        }
    }
}
