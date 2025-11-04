import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'cart_provider.dart';
import 'cart_screen.dart';
import 'data.dart';

class ProductListScreen extends StatelessWidget {
  const ProductListScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Food Order App'),
        actions: [
          Padding(
            padding: const EdgeInsets.all(10),
            child: IconButton(
              icon: const Icon(Icons.shopping_cart),
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => const CartScreen()),
                );
              },
            ),
          ),
        ],
      ),
      body: ListView.builder(
        itemCount: DUMMY_PRODUCTS.length,
        itemBuilder: (ctx, i) => ListTile(
          title: Text(DUMMY_PRODUCTS[i]['title'] as String),
          subtitle: Text('\$${DUMMY_PRODUCTS[i]['price']}'),
          trailing: IconButton(
            icon: const Icon(Icons.add),
            onPressed: () {
              Provider.of<Cart>(context, listen: false).addItem(
                DUMMY_PRODUCTS[i]['id'] as String,
                DUMMY_PRODUCTS[i]['price'] as double,
                DUMMY_PRODUCTS[i]['title'] as String,
              );
              ScaffoldMessenger.of(context).showSnackBar(
                const SnackBar(content: Text('Added To Cart')),
              );
            },
          ),
        ),
      ),
    );
  }
}
