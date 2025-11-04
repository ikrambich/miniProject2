import 'cart_model.dart';
import 'package:flutter/material.dart';

class Cart with ChangeNotifier {
  Map<String, CartItem> _items = {};

  Map<String, CartItem> get items {
    return {..._items};
  }

  int get itemCount {
    return _items.length;
  }

  double get totalAmount {
    double total = 0.0;
    _items.forEach((key, cartItem) {
      total += cartItem.price * cartItem.quantity;
    });
    return total;
  }

  void addItem(String productId, double price, String title) {
  if (_items.containsKey(productId)) {
    // If the item already exists, update its quantity
    _items.update(
      productId,
      (existingCartItem) => CartItem(
        id: existingCartItem.id,
        title: existingCartItem.title,
        price: existingCartItem.price,
        quantity: existingCartItem.quantity + 1,
      ),
    );
  } else {
    // If it's a new item, add it to the cart
    _items.putIfAbsent(
      productId,
      () => CartItem(
        id: DateTime.now().toString(),
        title: title,
        price: price,
        quantity: 1,
      ),
    );
  }

  // Notify listeners so the UI updates
  notifyListeners();
}
void removeItem(String productId) {
  _items.remove(productId);
  notifyListeners();
}

void removeSingleItem(String productId) {
  if (!_items.containsKey(productId)) {
    return;
  }

  if (_items[productId]!.quantity > 1) {
    _items.update(
      productId,
      (existingCartItem) => CartItem(
        id: existingCartItem.id,
        title: existingCartItem.title,
        price: existingCartItem.price,
        quantity: existingCartItem.quantity - 1,
      ),
    );
  } else {
    _items.remove(productId);
  }

  notifyListeners();
}
void clear() {
  _items = {};
  notifyListeners();
}


}
