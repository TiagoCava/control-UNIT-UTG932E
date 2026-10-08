Option Explicit On 

Imports NationalInstruments.VisaNS

Public Class SelectResource
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer
    Private WithEvents availableResourcesListBox As System.Windows.Forms.ListBox
    Private WithEvents okButton As System.Windows.Forms.Button
    Private WithEvents closeButton As System.Windows.Forms.Button
    Private WithEvents visaResourceNameTextBox As System.Windows.Forms.TextBox

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Private WithEvents AvailableResourcesLabel As System.Windows.Forms.Label
    Private WithEvents ResourceStringLabel As System.Windows.Forms.Label
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.availableResourcesListBox = New System.Windows.Forms.ListBox()
        Me.okButton = New System.Windows.Forms.Button()
        Me.closeButton = New System.Windows.Forms.Button()
        Me.visaResourceNameTextBox = New System.Windows.Forms.TextBox()
        Me.AvailableResourcesLabel = New System.Windows.Forms.Label()
        Me.ResourceStringLabel = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'availableResourcesListBox
        '
        Me.availableResourcesListBox.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.availableResourcesListBox.ItemHeight = 12
        Me.availableResourcesListBox.Location = New System.Drawing.Point(6, 19)
        Me.availableResourcesListBox.Name = "availableResourcesListBox"
        Me.availableResourcesListBox.Size = New System.Drawing.Size(280, 88)
        Me.availableResourcesListBox.TabIndex = 0
        '
        'okButton
        '
        Me.okButton.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.okButton.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.okButton.Location = New System.Drawing.Point(6, 191)
        Me.okButton.Name = "okButton"
        Me.okButton.Size = New System.Drawing.Size(92, 27)
        Me.okButton.TabIndex = 2
        Me.okButton.Text = "OK"
        '
        'closeButton
        '
        Me.closeButton.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.closeButton.Location = New System.Drawing.Point(98, 191)
        Me.closeButton.Name = "closeButton"
        Me.closeButton.Size = New System.Drawing.Size(93, 27)
        Me.closeButton.TabIndex = 3
        Me.closeButton.Text = "Cancel"
        '
        'visaResourceNameTextBox
        '
        Me.visaResourceNameTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.visaResourceNameTextBox.Location = New System.Drawing.Point(6, 159)
        Me.visaResourceNameTextBox.Name = "visaResourceNameTextBox"
        Me.visaResourceNameTextBox.Size = New System.Drawing.Size(280, 21)
        Me.visaResourceNameTextBox.TabIndex = 4
        Me.visaResourceNameTextBox.Text = "GPIB0::2::INSTR"
        '
        'AvailableResourcesLabel
        '
        Me.AvailableResourcesLabel.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.AvailableResourcesLabel.Location = New System.Drawing.Point(6, 5)
        Me.AvailableResourcesLabel.Name = "AvailableResourcesLabel"
        Me.AvailableResourcesLabel.Size = New System.Drawing.Size(277, 13)
        Me.AvailableResourcesLabel.TabIndex = 5
        Me.AvailableResourcesLabel.Text = "Available Resources:"
        '
        'ResourceStringLabel
        '
        Me.ResourceStringLabel.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ResourceStringLabel.Location = New System.Drawing.Point(6, 142)
        Me.ResourceStringLabel.Name = "ResourceStringLabel"
        Me.ResourceStringLabel.Size = New System.Drawing.Size(277, 14)
        Me.ResourceStringLabel.TabIndex = 6
        Me.ResourceStringLabel.Text = "Resource String:"
        '
        'SelectResource
        '
        Me.AcceptButton = Me.okButton
        Me.AutoScaleBaseSize = New System.Drawing.Size(6, 14)
        Me.CancelButton = Me.closeButton
        Me.ClientSize = New System.Drawing.Size(292, 227)
        Me.Controls.Add(Me.ResourceStringLabel)
        Me.Controls.Add(Me.AvailableResourcesLabel)
        Me.Controls.Add(Me.visaResourceNameTextBox)
        Me.Controls.Add(Me.closeButton)
        Me.Controls.Add(Me.okButton)
        Me.Controls.Add(Me.availableResourcesListBox)
        Me.KeyPreview = True
        Me.MaximizeBox = False
        Me.MinimumSize = New System.Drawing.Size(212, 266)
        Me.Name = "SelectResource"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Select Resource"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private Sub SelectResource_OnLoad(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim resources() As String = ResourceManager.GetLocalManager().FindResources("?*")
        Dim s As String
        For Each s In resources
            availableResourcesListBox.Items.Add(s)
        Next
        availableResourcesListBox.Items.Add("TCPIP[board]::host address[::LAN device name][::INSTR]")
        availableResourcesListBox.Items.Add("TCPIP[board]::host address::port::SOCKET")
    End Sub

    Private Sub availableResourcesListBox_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles availableResourcesListBox.DoubleClick
        Dim selectedString As String = CType(availableResourcesListBox.SelectedItem, String)
        ResourceName = selectedString
        Me.DialogResult = Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub

    Private Sub availableResourcesListBox_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles availableResourcesListBox.SelectedIndexChanged
        Dim selectedString As String = CType(availableResourcesListBox.SelectedItem, String)
        ResourceName = selectedString
    End Sub

    Public Property ResourceName() As String
        Get
            Return visaResourceNameTextBox.Text
        End Get
        Set(ByVal Value As String)
            visaResourceNameTextBox.Text = Value
        End Set
    End Property

End Class