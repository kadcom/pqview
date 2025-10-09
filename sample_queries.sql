-- Sample Queries for Testing the PostgreSQL Query Client
-- Use these during the stream to demonstrate various SQL features

-- ============================================
-- BASIC QUERIES (Hour 3 - Initial Testing)
-- ============================================

-- Simple SELECT all
SELECT * FROM customers;

-- SELECT with specific columns
SELECT first_name, last_name, email FROM customers;

-- SELECT with WHERE clause
SELECT * FROM products WHERE price > 50.00;

-- SELECT with ORDER BY
SELECT product_name, price FROM products ORDER BY price DESC;


-- ============================================
-- INTERMEDIATE QUERIES (Hour 4 - ListView Testing)
-- ============================================

-- COUNT and aggregate
SELECT category_name, COUNT(*) as product_count
FROM products p
JOIN categories c ON p.category_id = c.category_id
GROUP BY category_name;

-- JOIN query
SELECT
    c.first_name,
    c.last_name,
    o.order_id,
    o.order_date,
    o.total_amount
FROM customers c
JOIN orders o ON c.customer_id = o.customer_id
ORDER BY o.order_date DESC;

-- Multiple JOINs
SELECT
    o.order_id,
    c.first_name || ' ' || c.last_name as customer_name,
    p.product_name,
    oi.quantity,
    oi.unit_price
FROM orders o
JOIN customers c ON o.customer_id = c.customer_id
JOIN order_items oi ON o.order_id = oi.order_id
JOIN products p ON oi.product_id = p.product_id
ORDER BY o.order_id;


-- ============================================
-- ADVANCED QUERIES (Hour 5 - Stress Testing)
-- ============================================

-- Using views
SELECT * FROM customer_order_summary ORDER BY total_spent DESC;

-- Subquery
SELECT product_name, price
FROM products
WHERE price > (SELECT AVG(price) FROM products);

-- CASE statement
SELECT
    product_name,
    price,
    CASE
        WHEN price < 30 THEN 'Budget'
        WHEN price < 100 THEN 'Mid-range'
        ELSE 'Premium'
    END as price_category
FROM products;

-- Date functions
SELECT
    customer_id,
    first_name,
    last_name,
    registered_date,
    CURRENT_DATE - registered_date as days_registered
FROM customers;

-- String functions
SELECT
    UPPER(first_name) as first_name_upper,
    LOWER(last_name) as last_name_lower,
    LENGTH(email) as email_length,
    email
FROM customers;

-- NULL testing (no NULLs in this dataset, but good to test handling)
SELECT
    product_name,
    COALESCE(stock_quantity, 0) as stock
FROM products;

-- LIMIT and OFFSET (pagination)
SELECT * FROM customers ORDER BY customer_id LIMIT 5 OFFSET 0;
SELECT * FROM customers ORDER BY customer_id LIMIT 5 OFFSET 5;


-- ============================================
-- ERROR TESTING (Good for Debug Window)
-- ============================================

-- Syntax error (missing FROM)
-- SELECT * customers;

-- Non-existent table
-- SELECT * FROM non_existent_table;

-- Invalid column
-- SELECT invalid_column FROM customers;

-- Division by zero
-- SELECT 1/0;
