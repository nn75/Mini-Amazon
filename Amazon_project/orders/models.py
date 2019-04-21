from django.db import models


# Create your models here.
class Product(models.Model):
    product_id = models.IntegerField()
    product_name = models.TextField()
    wh_id = models.IntegerField()
    stock = models.IntegerField(default=500)

    class Meta:
        unique_together = (('product_id', 'wh_id'),)


class PendingOrder(models.Model):
    tracking_number = models.AutoField(primary_key=True)
    credit_card = models.CharField(max_length=16)
    user_id = models.IntegerField()
    ups_account = models.CharField(max_length=500, null=False, blank=False)
    product_name = models.CharField(max_length=500, null=False, blank=False)
    amount = models.IntegerField(default=1)
    address_x = models.IntegerField(null=False, blank=False)
    address_y = models.IntegerField(null=False, blank=False)


class Order(models.Model):
    tracking_number = models.AutoField(primary_key=True)
    user_id = models.IntegerField()
    ups_account = models.CharField(max_length=500, null=False, blank=False)
    product_id = models.IntegerField()
    wh_id = models.IntegerField()
    truck_id = models.IntegerField()
    status = models.CharField(max_length=50)
    adr_x = models.IntegerField(null=False, blank=False)
    adr_y = models.IntegerField(null=False, blank=False)

