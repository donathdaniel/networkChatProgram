   M S B A M L   `   `   `  ����A  =kliens, Version=1.0.0.0, Culture=neutral, PublicKeyToken=nullclr-namespace:klienskliens  5   	       kliens.MainWindow   5   	   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35e aSystem.Windows.Controls.Ribbon, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   @d2http://schemas.microsoft.com/expression/blend/2008     5   	   Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5   	   &localclr-namespace:kliens     5   	     � Title$  
MainWindow��5   	   $	��450��6   $	��800��6)   .��5	      � 
�������.+�5
   
   �� -   5        $��NameTextBox��5
      $
��Left=�6%   $��	30,35,0,0q�6@    �� TextWrapping$
 Wrap=�6S   $	��Top=�6g   $	��220��6   $��30��6�   �� 5   
   $
��Name��6   $
��Left=�6   $��	30,11,0,0q�6:   $	��Top=�6M   �� 5   
   -   5        $��ConnectButton��5      $��Connect��6B   $
��Left=�6T   $��
290,35,0,0q�6o   $	��Top=�6�   $	��220��6�   $��30��6�   �� 5   
   -   5        $��OutputListBox��5      $��30,80,270,124q�6'   5      �� 5   
   -   5        $��ConnectedNamesListBox��5      $��550,35,30,255q�6/   �� 5   
   -   5        $��SendTypeComboBox��5      $
��Left=�6+   $��550,160,0,0q�6F   $	��Top=�6[   $	��205��6s   .@�5      �� $��Public��6   
 � Tag$ 0��6,    �� 
IsSelected$
 True��64   �� 5      $��Private��6   $ 1��6-   �� 5      $��Group��6   $ 2��6+   �� 5      $
��File��6   $ 3��6*   �� 5      $��Image��6   $ 4��6+   5      �� 5   
   -   5        $��SendTextBox��5      $
��Left=�6%   $��550,190,0,0q�6@   $
 Wrap=�6U   $	��Top=�6i   $	��205��6�   $	��100��6�   �� 5   
   -   5        $��SendTextButton��5      ${�False��6D   $
��Send��6V   $
��Left=�6e   $��550,300,0,0q�6�   $	��Top=�6�   $��30��6�   $	��205��6�   �� 5   
   -   5        $��GroupNameTextBox��5      $
��Left=�6*   $��
30,300,0,0q�6E   $
 Wrap=�6Y   $	��Top=�6m   $	��100��6�   $��30��6�   �� 5   
   -	   5        $��CreateGroupButton��5      ${�False��6J   $��Create Group��6\   $
��Left=�6s   $��130,300,0,0q�6�   $	��Top=�6�   $��90��6�   $��30��6�   �� 5    
   -
   5        $��DeleteGroupButton��5       ${�False��6J   $��Delete Group��6\   $
��Left=�6s   $��220,300,0,0q�6�   $	��Top=�6�   $��90��6�   $��30��6�   �� 5!   
   -   5        $��AddToGroupButton��5!      ${�False��6H   $��Add to Group��6Z   $
��Left=�6q   $��310,300,0,0q�6�   $	��Top=�6�   $��90��6�   $��30��6�   �� 5"   
   -   5        $��RemoveFromGroupButton��5"      ${�False��6R   $��Remove from Group��6d   $
��Left=�6�   $��400,300,0,0q�6�   $	��Top=�6�   $	��115��6�   $��30��6�   5&      5'      