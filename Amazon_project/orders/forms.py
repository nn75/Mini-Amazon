from django import forms
from .models import PendingOrder


class SelectProductForm(forms.Form):
    product_name = forms.CharField(label='Product Name', max_length=500)
    amount = forms.IntegerField(label='Amount', max_value=99)


class PlaceOrderForm(forms.ModelForm):
    user_id = forms.IntegerField(widget=forms.TextInput(attrs={'readonly': 'readonly'}))
    product_name = forms.CharField(max_length=500, widget=forms.TextInput(attrs={'readonly': 'readonly'}))
    amount = forms.IntegerField(widget=forms.TextInput(attrs={'readonly': 'readonly'}))

    class Meta:
        model = PendingOrder
        fields = ['user_id', 'ups_account', 'product_name', 'amount', 'credit_card', 'address_x', 'address_y']


class QueryOrderForm(forms.Form):
    tracking_number = forms.IntegerField(label='Tracking Number')
