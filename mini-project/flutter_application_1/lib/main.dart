import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'cart_provider.dart';
import 'product_list_screen.dart';

void main() => runApp(const MyApp());

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (_) => Cart(),
      child: const MaterialApp(
        debugShowCheckedModeBanner: false,
        title: 'Food Order App',
        home: ProductListScreen(),
      ),
    );
  }
}
