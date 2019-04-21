from django.shortcuts import render, redirect
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.views.decorators.http import require_http_methods

from .forms import PlaceOrderForm, SelectProductForm, QueryOrderForm
from .models import Product, Order
from django.db.models import Sum
import socket


def send_data_to_server(payload):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('vcm-8252.vm.duke.edu', 45678))
    s.sendall(bytes(str(payload),  'utf-8'))
    s.close()


@login_required
def select_product(request):
    if request.method == 'POST':
        # create a form instance and populate it with data from the request:
        form = SelectProductForm(request.POST)
        if form.is_valid():
            product_name = form.cleaned_data['product_name']
            amount = form.cleaned_data['amount']
            # get total_stock of this product from all warehouses
            total_stock = Product.objects.filter(product_name=product_name).aggregate(Sum('stock'))
            # if stock of existing product is less than demand, return error msg
            if total_stock['stock__sum'] and total_stock['stock__sum'] < amount:
                messages.error(request, f'Low stock, we will pack soon.')
                return render(request, 'orders/select_product.html', {'form': form})
            else:
                return redirect('place_order', product_name=product_name, amount=amount)
    else:
        form = SelectProductForm()

    return render(request, 'orders/select_product.html', {'form': form})


@login_required
def place_order(request, product_name, amount):
    if request.method == 'POST':
        form = PlaceOrderForm(request.POST)
        if form.is_valid():
            tracking_number = form.save().tracking_number
            try:
                send_data_to_server(tracking_number+'/'+form.cleaned_data['ups_account'])
            except:
                print("Server not responsive.. abort transmission")
            return redirect('place_order_done')
    else:
        form = PlaceOrderForm(initial={
            'user_id': request.user.id,
            'product_name': product_name,
            'amount': amount
        })

    return render(request, 'orders/place_order.html', {'form': form})


@login_required
def place_order_done(request):
    return render(request, 'orders/place_order_done.html')


@login_required
@require_http_methods(["GET"])
def order_history(request):
    orders = Order.objects.filter(user_id=request.user.id)
    return render(request, 'orders/order_history.html', {'orders': orders})


@login_required
def query_order(request):
    if request.method == 'POST':
        form = QueryOrderForm(request.POST)
        if form.is_valid():
            tracking_number = form.cleaned_data['tracking_number']
            orders = Order.objects.filter(user_id=request.user.id).filter(tracking_number=tracking_number)
            if len(orders) == 0:
                return render(request, 'orders/query_order.html', {'form': form, 'message': 'No Results.'})
            else:
                return render(request, 'orders/query_order.html', {'form': form, 'orders': orders})

    else:
        form = QueryOrderForm()

    return render(request, 'orders/query_order.html', {'form': form, 'message': 'Please search by tracking numbers for results.'})

