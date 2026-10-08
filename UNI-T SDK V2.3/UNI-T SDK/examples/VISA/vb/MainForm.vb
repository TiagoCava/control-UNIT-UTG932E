Imports NationalInstruments.VisaNS

Public Class MainForm
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()
        InitializeComponent()
    End Sub
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    Private components As System.ComponentModel.IContainer
    Private mbSession As MessageBasedSession
    Private lastResourceString As String = Nothing
    Private WithEvents writeTextBox As System.Windows.Forms.TextBox
    Private WithEvents readTextBox As System.Windows.Forms.TextBox

    Private WithEvents queryButton As System.Windows.Forms.Button
    Private WithEvents writeButton As System.Windows.Forms.Button
    Private WithEvents readButton As System.Windows.Forms.Button
    Private WithEvents openSessionButton As System.Windows.Forms.Button
    Private WithEvents clearButton As System.Windows.Forms.Button
    Private WithEvents closeSessionButton As System.Windows.Forms.Button
    Private WithEvents stringToWriteLabel As System.Windows.Forms.Label
    Private WithEvents stringToReadLabel As System.Windows.Forms.Label
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.queryButton = New System.Windows.Forms.Button()
        Me.writeButton = New System.Windows.Forms.Button()
        Me.readButton = New System.Windows.Forms.Button()
        Me.openSessionButton = New System.Windows.Forms.Button()
        Me.writeTextBox = New System.Windows.Forms.TextBox()
        Me.readTextBox = New System.Windows.Forms.TextBox()
        Me.clearButton = New System.Windows.Forms.Button()
        Me.closeSessionButton = New System.Windows.Forms.Button()
        Me.stringToWriteLabel = New System.Windows.Forms.Label()
        Me.stringToReadLabel = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        '查询
        '
        Me.queryButton.Enabled = False
        Me.queryButton.Location = New System.Drawing.Point(6, 89)
        Me.queryButton.Name = "queryButton"
        Me.queryButton.Size = New System.Drawing.Size(89, 25)
        Me.queryButton.TabIndex = 3
        Me.queryButton.Text = "Query"
        '
        '写
        '
        Me.writeButton.Enabled = False
        Me.writeButton.Location = New System.Drawing.Point(95, 89)
        Me.writeButton.Name = "writeButton"
        Me.writeButton.Size = New System.Drawing.Size(89, 25)
        Me.writeButton.TabIndex = 4
        Me.writeButton.Text = "Write"
        '
        '读
        '
        Me.readButton.Enabled = False
        Me.readButton.Location = New System.Drawing.Point(184, 89)
        Me.readButton.Name = "readButton"
        Me.readButton.Size = New System.Drawing.Size(88, 25)
        Me.readButton.TabIndex = 5
        Me.readButton.Text = "Read"
        '
        '打开
        '
        Me.openSessionButton.Location = New System.Drawing.Point(6, 5)
        Me.openSessionButton.Name = "openSessionButton"
        Me.openSessionButton.Size = New System.Drawing.Size(110, 24)
        Me.openSessionButton.TabIndex = 0
        Me.openSessionButton.Text = "Open Session"
        '
        '写指令
        '
        Me.writeTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.writeTextBox.Enabled = False
        Me.writeTextBox.Location = New System.Drawing.Point(6, 58)
        Me.writeTextBox.Name = "writeTextBox"
        Me.writeTextBox.Size = New System.Drawing.Size(324, 21)
        Me.writeTextBox.TabIndex = 2
        Me.writeTextBox.Text = "*IDN?\n"
        '
        '读缓冲
        '
        Me.readTextBox.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.readTextBox.Location = New System.Drawing.Point(6, 146)
        Me.readTextBox.Multiline = True
        Me.readTextBox.Name = "readTextBox"
        Me.readTextBox.ReadOnly = True
        Me.readTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.readTextBox.Size = New System.Drawing.Size(324, 119)
        Me.readTextBox.TabIndex = 6
        Me.readTextBox.TabStop = False
        '
        '清空
        '
        Me.clearButton.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.clearButton.Enabled = False
        Me.clearButton.Location = New System.Drawing.Point(7, 267)
        Me.clearButton.Name = "clearButton"
        Me.clearButton.Size = New System.Drawing.Size(324, 26)
        Me.clearButton.TabIndex = 7
        Me.clearButton.Text = "Clear"
        '
        '关闭资源
        '
        Me.closeSessionButton.Enabled = False
        Me.closeSessionButton.Location = New System.Drawing.Point(116, 5)
        Me.closeSessionButton.Name = "closeSessionButton"
        Me.closeSessionButton.Size = New System.Drawing.Size(111, 24)
        Me.closeSessionButton.TabIndex = 1
        Me.closeSessionButton.Text = "Close Session"
        '
        '写标题
        '
        Me.stringToWriteLabel.Location = New System.Drawing.Point(6, 37)
        Me.stringToWriteLabel.Name = "stringToWriteLabel"
        Me.stringToWriteLabel.Size = New System.Drawing.Size(109, 15)
        Me.stringToWriteLabel.TabIndex = 8
        Me.stringToWriteLabel.Text = "String to Write:"
        '
        '读标题
        '
        Me.stringToReadLabel.Location = New System.Drawing.Point(6, 126)
        Me.stringToReadLabel.Name = "stringToReadLabel"
        Me.stringToReadLabel.Size = New System.Drawing.Size(121, 16)
        Me.stringToReadLabel.TabIndex = 9
        Me.stringToReadLabel.Text = "String to Read:"
        '
        '主界面
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(6, 14)
        Me.ClientSize = New System.Drawing.Size(338, 301)
        Me.Controls.Add(Me.stringToReadLabel)
        Me.Controls.Add(Me.stringToWriteLabel)
        Me.Controls.Add(Me.closeSessionButton)
        Me.Controls.Add(Me.clearButton)
        Me.Controls.Add(Me.readTextBox)
        Me.Controls.Add(Me.writeTextBox)
        Me.Controls.Add(Me.openSessionButton)
        Me.Controls.Add(Me.readButton)
        Me.Controls.Add(Me.writeButton)
        Me.Controls.Add(Me.queryButton)
        Me.MaximizeBox = False
        Me.MinimumSize = New System.Drawing.Size(354, 340)
        Me.Name = "MainForm"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Simple Read/Write"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    <STAThread()> _
    Public Shared Sub Main()
        Application.Run(New MainForm)
    End Sub


    Private Sub openSession_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles openSessionButton.Click
        Dim sr As SelectResource = New SelectResource
        If lastResourceString <> Nothing Then
            sr.ResourceName = lastResourceString
        End If
        Dim result As DialogResult = sr.ShowDialog(Me)
        If result = Windows.Forms.DialogResult.OK Then
            lastResourceString = sr.ResourceName
            Windows.Forms.Cursor.Current = Cursors.WaitCursor
            Try
                mbSession = CType(ResourceManager.GetLocalManager().Open(sr.ResourceName), MessageBasedSession)
                SetupControlState(True)
            Catch exp As InvalidCastException
                MessageBox.Show("Resource selected must be a message-based session")
            Catch exp As Exception
                MessageBox.Show(exp.Message)
            Finally
                Windows.Forms.Cursor.Current = Cursors.Default
            End Try
        End If
        sr.Dispose()
    End Sub

    Private Sub closeSession_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles closeSessionButton.Click
        SetupControlState(False)
        mbSession.Dispose()
    End Sub

    Private Sub query_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles queryButton.Click
        Windows.Forms.Cursor.Current = Cursors.WaitCursor
        Try
            Dim textToWrite As String = ReplaceCommonEscapeSequences(writeTextBox.Text)
            Dim responseString As String = mbSession.Query(textToWrite)
            readTextBox.Text = InsertCommonEscapeSequences(responseString)
        Catch exp As Exception
            MessageBox.Show(exp.Message)
        Finally
            Windows.Forms.Cursor.Current = Cursors.Default
        End Try
    End Sub

    Private Sub write_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles writeButton.Click
        Try
            Dim textToWrite As String = ReplaceCommonEscapeSequences(writeTextBox.Text)
            mbSession.Write(textToWrite)
        Catch exp As Exception
            MessageBox.Show(exp.Message)
        End Try
    End Sub

    Private Sub read_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles readButton.Click
        Windows.Forms.Cursor.Current = Cursors.WaitCursor
        Try
            Dim responseString As String = mbSession.ReadString()
            readTextBox.Text = InsertCommonEscapeSequences(responseString)
        Catch exp As Exception
            MessageBox.Show(exp.Message)
        Finally
            Windows.Forms.Cursor.Current = Cursors.Default
        End Try
    End Sub

    Private Sub clear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles clearButton.Click
        readTextBox.Text = String.Empty
    End Sub

    Private Sub SetupControlState(ByVal isSessionOpen As Boolean)
        openSessionButton.Enabled = Not isSessionOpen
        closeSessionButton.Enabled = isSessionOpen
        queryButton.Enabled = isSessionOpen
        writeButton.Enabled = isSessionOpen
        readButton.Enabled = isSessionOpen
        writeTextBox.Enabled = isSessionOpen
        clearButton.Enabled = isSessionOpen
        If isSessionOpen Then
            readTextBox.Text = String.Empty
            writeTextBox.Focus()
        End If
    End Sub

    Private Function ReplaceCommonEscapeSequences(ByVal s As String) As String
        Return s.Replace("\n", vbLf).Replace("\r", vbCr)
    End Function

    Private Function InsertCommonEscapeSequences(ByVal s As String) As String
        Return s.Replace(vbLf, "\n").Replace(vbCr, "\r")
    End Function

End Class